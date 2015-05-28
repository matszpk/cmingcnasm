#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/stat.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/ascii/block/conv/decimal/decimal.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u8 ulinux_u8
#define s8 ulinux_s8
#define u32 ulinux_u32
#define s32 ulinux_s32
#define u64 ulinux_u64
#define s64 ulinux_s64
#define ISERR ULINUX_ISERR
#define stat ulinux_stat
#define PROT_READ ULINUX_PROT_READ
#define MAP_PRIVATE ULINUX_MAP_PRIVATE
#define MAP_POPULATE ULINUX_MAP_POPULATE
#define strncmp ulinux_strncmp
#define dec2u32_blk ulinux_dec2u32_blk
#define O_RDONLY ULINUX_O_RDONLY
#define open(path,flgs,more) ulinux_sysc(open,3,path,flgs,more)
#define fstat(fd,stat) ulinux_sysc(fstat,2,fd,stat)
#define mmap(addr,sz,attr,flgs,fd,off) ulinux_sysc(mmap,6,addr,sz,attr,flgs,fd,\
off)
#define close(fd) ulinux_sysc(close,1,fd)
#define munmap(addr,sz) ulinux_sysc(munmap,2,addr,sz)
#define exit(code) ulinux_sysc(exit_group,1,code)
//------------------------------------------------------------------------------

#define CONF_INI_ERR -1
#define CONF_INI_OK   0

#define DPRINTF_BUF_SZ 512

#define cs_n(x) (sizeof(x)-1)

#define PERRC(str) {l rl;do{rl=ulinux_sysc(write,3,2,str,cs_n(str));}\
                         while(rl==-EINTR||rl==-EAGAIN);}
#define PERR(fmt,...) ulinux_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
                                ##__VA_ARGS__)
#define POUT(fmt,...) ulinux_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
                                ##__VA_ARGS__)
#define POUTC(str) {l rl;do{rl=ulinux_sysc(write,3,1,str,cs_n(str));}\
                         while(rl==-EINTR||rl==-EAGAIN);}

static u8 *dprintf_buf;

//******************************************************************************
//data structs
struct ctx{
  i fd;
  void *m;
  u64 sz;
};

struct ctx_mysection{
  //here should go some variable to match properly a mysection
  struct ctx c;
};
//******************************************************************************

//******************************************************************************
//generic parse functions
static s64 blanks_skip(struct ctx *c,s64 start,s64 line_end)
{
    while(1){
      if(start==line_end) break;
      u8 start_char=*(u8*)(c->m+start);
      if(start_char!=' '&&start_char!='\t') break;
      ++start;
    }
    return start;
}

static s64 non_blanks_skip(struct ctx *c,s64 start,s64 line_end)
{
    while(1){
      if(start==line_end) break;
      u8 start_char=*(u8*)(c->m+start);
      if(start_char==' '||start_char=='\t') break;
      ++start;
    }
    return start;
}

#define KEY_SKIP_TO_VAL_NO_VAL -1
static s64 key_skip_to_val(struct ctx *c,s64 start, s64 line_end)
{
    while(1){
      if(start==line_end) break;
      if(*(u8*)(c->m+start)=='=') break;
      ++start;
    }
    if(start==line_end) return KEY_SKIP_TO_VAL_NO_VAL;
    s64 val_start=start+1;
    if(val_start==line_end) return KEY_SKIP_TO_VAL_NO_VAL;
    return val_start;
}

//line_end is the offset of the terminating '/n' or of the byte right after
//the last file byte
static s64 line_end_reach(struct ctx *c,s64 line_start)
{
  s64 line_end=line_start;
  while(1){
    if((u64)line_end==c->sz||*(u8*)(c->m+line_end)=='\n') return line_end;
    ++line_end;
  }
}
//end generic parse functions
//******************************************************************************

//******************************************************************************
//file management related functions
static s8 file_open_ro(struct ctx *c,void *path)
{
  i fd;
  do fd=open(path,O_RDONLY,0); while(fd==-EINTR);
  if(ISERR(fd)){
    PERR("file:error(%d):unable to open conf file %s\n",fd,path);
    goto err;
  }

  struct stat cfg_stat;
  l r=fstat(fd,&cfg_stat);
  if(ISERR(r)){
    PERR("file:error(%ld):unable to stat conf file %s\n",r,path);
    goto err_close_fd;
  }

  l addr=mmap(0,cfg_stat.sz,PROT_READ,MAP_PRIVATE|MAP_POPULATE,fd,0);
  if(!addr||ISERR(addr)){
    PERR("file:error(%ld):unable to mmap conf file %s\n",addr,path);
    goto err_close_fd;
  }

  c->fd=fd;
  c->m=(void*)addr;
  c->sz=(u64)cfg_stat.sz;

  POUT("file:conf file %s opened and mmaped\n",path);
  return CONF_INI_OK;

err_close_fd:
  do r=close(fd); while(r==-EINTR);
  if(ISERR(r))
    PERR("file:error(%ld):unable to close conf file\n",r,path);
err:
  return CONF_INI_ERR;
}

static s8 file_close(struct ctx *c)
{
  s8 r0=CONF_INI_OK;

  l r=munmap(c->m,c->sz);
  if(ISERR(r)){
    PERR("file:error(%ld):unable to munmap conf file\n",r);
    r0=CONF_INI_ERR;goto exit;
  }

  do r=close(c->fd); while(r==-EINTR);
  if(ISERR(r)){
    PERR("file:error(%ld):closing conf file gone wrong\n",r);
    r0=CONF_INI_ERR;goto exit;
  }

  POUTC("file:conf file unmapped and closed\n");
exit:
  return r0;
}
//end file management related functions
//******************************************************************************

//******************************************************************************
//parse section properties
static void *string_property_get(struct ctx_mysection *cm,s64 val_start,
                                                                   s64 line_end)
{
  POUTC("string property found\n");
  void *r="not valid value";

  s64 val_end=non_blanks_skip(&cm->c,val_start,line_end);

  if((val_end-val_start)==cs_n("value0")&&!strncmp(cm->c.m+val_start,"value0",
                                                             val_end-val_start))
    r="value0";
  else if((val_end-val_start)==cs_n("value1")&&!strncmp(cm->c.m+val_start,
                                                    "value1",val_end-val_start))
    r="value1";
  return r;
}

static s32 integer_property_get(struct ctx_mysection *cm,s64 val_start,
                                                                   s64 line_end)
{
  POUTC("integer property found\n");
  s32 r0=0;

  s64 val_end=non_blanks_skip(&cm->c,val_start,line_end);

  u32 integer;
  u8 r1=dec2u32_blk(&integer,cm->c.m+val_start,cm->c.m+val_end-1);
  if(!r1){
    PERRC("integer not a valid number\n");
    goto exit;
  }
  if(integer>0x7fffffff){
    PERR("integer(%u) above max(%u)\n",integer,0x7fffffff);
    goto exit;
  }
  r0=(s32)integer;
exit:
  return r0;
}

#define STRING_PROPERTY_KEY_STR "string"
#define INTEGER_PROPERTY_KEY_STR "integer"

#define IS_KEY(x) (val_start-1-key_start)==cs_n(x ## _KEY_STR)\
&&!strncmp(cm->c.m+key_start,x ## _KEY_STR,val_start-1-key_start)
static void mysection_mycategory_properties_get(struct ctx_mysection *cm,
                                                            s64 mysection_start)
{
  void *string_property="not found";
  u32 integer_property=0;

  s64 line_start=mysection_start;
  while(1){
    if((u64)line_start>=cm->c.sz){
      POUTC("no more mycategory properties:line start not in file\n");
      break;
    }

    s64 line_end=line_end_reach(&cm->c,line_start);
    s64 key_start=blanks_skip(&cm->c,line_start,line_end);

    //finished:beginning of next section reached ("[key]")
    if(*(u8*)(cm->c.m+key_start)=='[') break;

    s64 val_start=key_skip_to_val(&cm->c,key_start,line_end);
    if(val_start==KEY_SKIP_TO_VAL_NO_VAL){
      POUTC("no value\n");
    }else{
      if(IS_KEY(STRING_PROPERTY)) 
        string_property=string_property_get(cm,val_start,line_end);
      else if(IS_KEY(INTEGER_PROPERTY))
        integer_property=integer_property_get(cm,val_start,line_end);
      //add more properties in mycategory here
    }

    line_start=line_end+1;
  }
  POUT("CONF:string_property=%s integer_property=%d\n",string_property,
                                                              integer_property);
}
//end parse section properties
//******************************************************************************

//******************************************************************************
//parse conf file sections
#define MYSECTION_STR "[mysection]"
#define NO_MORE_MYSECTIONS -1
static s64 next(struct ctx_mysection *cm,s64 line_start)
{
  while(1){
    if((u64)line_start>=cm->c.sz){
      PERRC("no more output section:line start not in file\n");
      return NO_MORE_MYSECTIONS;
    }

    s64 line_end=line_end_reach(&cm->c,line_start);
    s64 key_start=blanks_skip(&cm->c,line_start,line_end);
    s64 key_end=non_blanks_skip(&cm->c,key_start,line_end);
    
    if(key_end-key_start==cs_n(MYSECTION_STR)&&!strncmp(cm->c.m+key_start,
                                              MYSECTION_STR,key_end-key_start)){
        PERR("mysection found at %ld\n",key_start);
        return line_end+1;
    }

    line_start=line_end+1;
  }
}

#define MATCH_ERR -1
#define NO_MATCH   0
#define MATCH      1
static s8 match(struct ctx_mysection *cm,s64 mysection_start)
{
  //XXX:in the meantime we match all the time
  (void)cm;(void)mysection_start;
  POUTC("unimplemented->we always match the first mysection\n");
  return MATCH;
}

#define LOOKUP_ERR       -2
#define LOOKUP_NOT_FOUND -1
static s64 mysection_lookup(struct ctx_mysection *cm)
{
  s64 mysection_start=0;
  while(1){
    mysection_start=next(cm,mysection_start);
    if(mysection_start==NO_MORE_MYSECTIONS){
      POUTC("no matching mysection was found\n");
      return LOOKUP_NOT_FOUND;
    }

    s8 r=match(cm,mysection_start);
    if(r==MATCH){
      POUTC("a matching mysection was found\n");
      return mysection_start;
    }
    if(r==MATCH_ERR){
      return LOOKUP_ERR;
    }
  }
}
//parse conf file sections
//******************************************************************************

void ulinux_start(l argc,void **argv)
{
  u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  if(argc!=2){
    PERR("args:error:missing path name argument for conf file(argc=%d)\n",argc);
    exit(-1);
  }

  struct ctx_mysection cm;
  cm.c.fd=-1;
  cm.c.m=0;
  cm.c.sz=0;

  s8 r=file_open_ro(&cm.c,argv[1]);
  if(r==CONF_INI_ERR) exit(-1);

  s64 mysection_start=mysection_lookup(&cm);
  if(mysection_start>=0){
    mysection_mycategory_properties_get(&cm,mysection_start);
    //here you can add other property categories
  }

  file_close(&cm.c);//ignore errors
  exit(0);
}
