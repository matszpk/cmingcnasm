#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/args_env.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#define BUF_SZ 2048
void start(k_i argc,k_u8 **argv_envp) 
{
  k_u8 buf[BUF_SZ];
  u_a_dprintf(1,buf,BUF_SZ,(k_u8*)"argc=%d,argv_envp=%p\n",argc,argv_envp);
  k_i env=argc+1;
  while(argc--)
    u_a_dprintf(1,buf,BUF_SZ,(k_u8*)"argv[%d]=%s\n",argc,argv_envp[argc]);
  while(argv_envp[env]){
    u_a_dprintf(1,buf,BUF_SZ,(k_u8*)"envp[%d]=%s\n",env,argv_envp[env]);
    ++env;
  }
  //in theory, linux or the linux loader put the elf extension after the env
  sysc(exit_group,1,0);
  __builtin_unreachable();
}
