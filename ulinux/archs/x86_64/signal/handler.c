/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ULINUX_ARCH_C
/*
signal handler setup is arch depend
on x86_84, a syscall restorer must be called and that must be rt_sigreturn
in order to get a usable stack-->naked syscall 15, namely rt_sigreturn
gdb won't be able to deal with the signal handler
*/
extern void restore_rt(void) asm ("__restore_rt");
asm(
  ".text\n"
  "__restore_rt:\n"
  "  movq  $15,%rax\n"
  "  syscall\n");

ulinux_sl arch_sigaction(ulinux_sl num,struct ulinux_sigaction *kact,
                                                 struct ulinux_sigaction *okact)
{
  kact->sa_flags|=SA_RESTORER;/**MUST* have a restorer on x86_64, see above*/
  kact->sa_restorer=restore_rt;/*bear rt_sigreturn syscall*/
  return sysc(rt_sigaction,4,num,kact,okact,sizeof(ulinux_ul));
}
