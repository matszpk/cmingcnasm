#ifndef ULINUX_ARCH_SIGNAL_SIGINFO_H
#define ULINUX_ARCH_SIGNAL_SIGINFO_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
union ulinux_sigval{
  ulinux_si i;
  void* ptr;
};

#define ULINUX_SI_PREAMBLE_SIZE (4*sizeof(ulinux_si))
#define ULINUX_SI_MAX_SIZE	128
#define ULINUX_SI_PAD_SIZE	((ULINUX_SI_MAX_SIZE-ULINUX_SI_PREAMBLE_SIZE)\
/sizeof(ulinux_si))

struct ulinux_siginfo{
  ulinux_si si_signo;
  ulinux_si si_errno;
  ulinux_si si_code;

  union{
    ulinux_si pad[ULINUX_SI_PAD_SIZE];

    /*kill()*/
    struct{
      ulinux_si pid;/*sender's pid*/
      ulinux_ui uid;/*sender's uid*/
    } kill;

    /*posix.1b timers*/
    struct{
      ulinux_si tid;/*timer id*/
      ulinux_si overrun;/*overrun count*/
      union ulinux_sigval sigval;/*same as below*/
      ulinux_si sys_private;/*not to be passed to user*/
    } timer;

    /*posix.1b signals*/
    struct{
      ulinux_si pid;/*sender's pid*/
      ulinux_ui uid;/*sender's uid*/
      union ulinux_sigval sigval;
    } rt;

    /*SIGCHLD*/
    struct{
      ulinux_si pid;/*which child*/
      ulinux_us uid;/*sender's uid*/
      ulinux_si status;/*exit code*/
      ulinux_sl utime;
      ulinux_sl stime;
    } sigchld;

    /*SIGILL, SIGFPE, SIGSEGV, SIGBUS*/
    struct{
      void* addr;/*faulting insn/memory ref.*/
      ulinux_ss addr_lsb;/*lsb of the reported address*/
    } sigfault;

    /*SIGPOLL*/
    struct{
      ulinux_sl band;/*POLL_IN, POLL_OUT, POLL_MSG*/
      ulinux_si fd;
    } sigpoll;
  } fields;
};
#endif
