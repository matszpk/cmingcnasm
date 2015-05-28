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
#include <ulinux/utils/ascii/string/vsprintf.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u8 ulinux_u8
#define exit(a) ulinux_sysc(exit_group,1,a)
#define open(a,b,c) ulinux_sysc(open,3,a,b,c)
#define RDONLY ULINUX_O_RDONLY
#define ISERR ULINUX_ISERR
#define stat ulinux_stat
#define fstat(a,b) ulinux_sysc(fstat,2,a,b)
#define mmap(a,b,c,d,e,f) ulinux_sysc(mmap,6,a,b,c,d,e,f)
#define PROT_READ ULINUX_PROT_READ
#define MAP_PRIVATE ULINUX_MAP_PRIVATE
#define MAP_POPULATE ULINUX_MAP_POPULATE
#define init_module(a,b,c) ulinux_sysc(init_module,3,a,b,c)
//------------------------------------------------------------------------------

#define DPRINTF_BUF_SZ 1024
static u8 *dprintf_buf;

#define PERRC(str) {l rl;do{rl=ulinux_sysc(write,3,2,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

#define PERR(fmt,...) ulinux_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
##__VA_ARGS__)

#define POUT(fmt,...) ulinux_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
##__VA_ARGS__)

#define POUTC(str) {l rl;do{rl=ulinux_sysc(write,3,1,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

void ulinux_start(l argc,void **argv)
{
  u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  if(argc!=2){
    PERR("ERROR:wrong number of command arguments(%d)\n",argc);
    exit(-1);
  }

  i fd;
  do fd=(i)open(argv[1],RDONLY,0); while(fd==-EINTR);
  if(ISERR(fd)){
    PERR("ERROR(%ld):unable to open module %s\n",fd,argv[1]);
    exit(-1);
  }

  struct stat m_stat;
  l r=fstat(fd,&m_stat);
  if(ISERR(r)){
    PERR("ERROR(%ld):unable to stat module\n",r);
    exit(-1);
  }
  POUT("size=%lu\n",m_stat.sz);

  l addr=mmap(0,m_stat.sz,PROT_READ,MAP_PRIVATE|MAP_POPULATE,fd,0);
  if(!addr||ISERR(addr)){
    PERR("ERROR(%ld):unable to mmap module file\n",addr);
    exit(-1);
  }

  r=init_module(addr,m_stat.sz,"");
  if(ISERR(r)){
    PERR("ERROR(%ld):unable init module\n",r);
    exit(-1);
  }
  exit(0);
}
