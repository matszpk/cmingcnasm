#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/fs.h>
#include <ulinux/dirent.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

/*----------------------------------------------------------------------------*/
/*ulinux namespace*/
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u8 ulinux_u8
#define s64 ulinux_s64
#define dirent64 ulinux_dirent64
#define getdents64(fd,dirents,sz) ulinux_sysc(getdents64,3,fd,dirents,sz)
#define ISERR ULINUX_ISERR
#define exit(code) ulinux_sysc(exit_group,1,code)
#define DT_DIR ULINUX_DT_DIR
#define openat(fd,name,flgs) ulinux_sysc(openat,4,fd,name,flgs,0)
#define open(path,flgs) ulinux_sysc(open,3,path,flgs,0)
#define RDONLY ULINUX_O_RDONLY
#define NONBLOCK ULINUX_O_NONBLOCK
#define close(fd) ulinux_sysc(close,1,fd)
#define loop while(1)
/*----------------------------------------------------------------------------*/

#define DPRINTF_BUF_SZ 1024
static u8 *dprintf_buf;

#define cs_n(x) (sizeof(x)-1)

#define PERRC(str)                                                             \
{                                                                              \
  l rl;                                                                        \
  loop{                                                                        \
    rl=ulinux_sysc(write,3,2,str,cs_n(str));                                   \
    if(rl!=-EINTR&&rl!=-EAGAIN) break;                                         \
  }                                                                            \
}
  
#define PERR(fmt,...) ulinux_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,\
(ulinux_u8*)fmt, ##__VA_ARGS__)

#define POUT(fmt,...) ulinux_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,\
(ulinux_u8*)fmt,##__VA_ARGS__)

#define POUTC(str)                                                             \
{                                                                              \
  l rl;                                                                        \
  loop{                                                                        \
    rl=ulinux_sysc(write,3,1,str,cs_n(str));                                   \
    if(rl!=-EINTR&&rl!=-EAGAIN) break;                                         \
  }                                                                            \
}

static u8 is_current(u8 *n)
{
  if(n[0]=='.'&&n[1]==0) return 1;
  return 0;
}

static u8 is_parent(u8 *n)
{
  if(n[0]=='.'&&n[1]=='.'&&n[2]==0) return 1;
  return 0;
}

static s64 depth=-1;

static void dout(struct dirent64 *d)
{
  s64 j;

  POUT("%20lu %20ld %2u ",d->ino,d->off,d->type);
  j=depth;
  loop{
    if(j==0) break;
    j--;
    POUTC("  ");
  }
  POUT("%s\n",d->name);
}

#define DIRENTS_BUF_SZ 8192
/*XXX:carefull, the dentry type is not supported by all fs*/
static void dir_parse(i parent_fd)
{
  u8 dirents[DIRENTS_BUF_SZ];

  ++depth;
  loop{
    l r;
    l j;

    r=getdents64(parent_fd,dirents,DIRENTS_BUF_SZ);
    if(ISERR(r)){
      PERR("ERROR(%ld):getdents error\n",r);
      exit(-1);
    }

    if(!r) break;

    j=0;
    loop{
      struct dirent64 *d;

      if(j>=r) break;

      d=(struct dirent64*)(dirents+j);

      dout(d);

      if(d->type==DT_DIR&&!is_current(d->name)&&!is_parent(d->name)){
        i dir_fd;

        loop{
          dir_fd=(i)openat(parent_fd,d->name,RDONLY|NONBLOCK);
          if(dir_fd!=-EINTR) break;
        }
        if(ISERR(dir_fd))
          PERR("ERROR(%d):unable to open subdir:%s\n",dir_fd,d->name);
        else{
          dir_parse(dir_fd);
          loop{
            l r1;

            r1=close(dir_fd);
            if(r1!=-EINTR) break;
          }
        }
      }
      j+=d->rec_len;
    }
  }
  depth--;
}

void ulinux_start(l argc,ulinux_u8 **argv)
{
  u8 _dprintf_buf[DPRINTF_BUF_SZ];
  i root_fd;

  dprintf_buf=&_dprintf_buf[0];

  if(argc!=2){
    PERR("ERROR:wrong number of command arguments(%d)\n",argc);
    exit(-1);
  }

  /*root fd*/
  loop{
    root_fd=(i)open(argv[1],RDONLY|NONBLOCK);
    if(root_fd!=-EINTR) break;
  }
  if(ISERR(root_fd)){
    PERR("ERROR(%d):unable to open root dir:%s\n",root_fd,argv[1]);
    exit(-1);
  }

  dir_parse(root_fd); 
  exit(0);
}
