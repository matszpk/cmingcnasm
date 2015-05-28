#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/args_env.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#define BUF_SZ 2048
void ulinux_start(ulinux_sl argc,void **argv_envp) 
{
  ulinux_u8 buf[BUF_SZ];
  ulinux_dprintf(1,buf,BUF_SZ,"argc=%d,argv_envp=%p\n",argc,argv_envp);
  ulinux_si env=argc+1;
  while(argc--)
    ulinux_dprintf(1,buf,BUF_SZ,"argv[%d]=%s\n",argc,argv_envp[argc]);
  while(argv_envp[env]){
    ulinux_dprintf(1,buf,BUF_SZ,"envp[%d]=%s\n",env,argv_envp[env]);
    ++env;
  }
  //in theory, linux or the linux loader put the elf extension after the env
  ulinux_sysc(exit_group,1,0);
}
