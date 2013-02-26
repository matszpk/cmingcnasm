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

#define DPRINTF_BUF_SZ 1024
static k_u8 *dprintf_buf;

#define PERRC(s) {k_l rl;do{rl=sysc(write,3,2,s,sizeof(s));}\
                         while(rl==-K_EINTR||rl==-K_EAGAIN);}
#define PERR(f,...) u_a_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
                                ##__VA_ARGS__)
#define POUT(f,...) u_a_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
                                __VA_ARGS__)
#define POUTC(s) {k_l rl;do{rl=sysc(write,3,1,s,sizeof(s));}\
                         while(rl==-K_EINTR||rl==-K_EAGAIN);}

void start(k_i argc,k_u8 **argv)
{
  k_u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  if(argc!=2){
    PERR("ERROR:wrong number of command arguments(%d)\n",argc);
    sysc(exit_group,1,-1);
  }

  k_i fd=sysc(open,3,argv[1],K_O_RDONLY,0);
  if(K_ISERR(fd)){
    PERR("ERROR(%ld):unable to open module %s\n",fd,argv[1]);
    sysc(exit_group,1,-1);
  }

  struct k_stat m_stat;
  k_l r=sysc(fstat,2,fd,&m_stat);
  if(K_ISERR(r)){
    PERR("ERROR(%ld):unable to stat module\n",r);
    sysc(exit_group,1,-1);
  }
  POUT("size=%lu\n",m_stat.sz);

  k_l addr=sysc(mmap,6,0,m_stat.sz,K_PROT_READ,
                       K_MAP_PRIVATE|K_MAP_POPULATE,fd,0);
  if(K_ISERR(addr)){
    PERR("ERROR(%ld):unable to mmap module file\n",addr);
    sysc(exit_group,1,-1);
  }

  r=sysc(init_module,3,addr,m_stat.sz,"");
  if(K_ISERR(r)){
    PERR("ERROR(%ld):unable init module\n",r);
    sysc(exit_group,1,-1);
  }
  sysc(exit_group,1,0);
}
