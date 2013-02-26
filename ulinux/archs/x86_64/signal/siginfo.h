#ifndef ULINUX_ARCH_SIGNAL_SIGINFO_H
#define ULINUX_ARCH_SIGNAL_SIGINFO_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
union k_sigval{
  k_i i;
  void* ptr;
};

#define SI_PREAMBLE_SIZE (4*sizeof(k_i))
#define SI_MAX_SIZE	128
#define SI_PAD_SIZE	((SI_MAX_SIZE-SI_PREAMBLE_SIZE)/sizeof(k_i))

struct k_siginfo{
  k_i si_signo;
  k_i si_errno;
  k_i si_code;

  union{
    k_i pad[SI_PAD_SIZE];

    //kill()
    struct{
      k_i pid;//sender's pid 
      k_u uid;//sender's uid
    } kill;

    //posix.1b timers
    struct{
      k_i tid;//timer id
      k_i overrun;//overrun count
      union k_sigval sigval;//same as below
      k_i sys_private;//not to be passed to user
    } timer;

    //posix.1b signals
    struct{
      k_i pid;//sender's pid
      k_u uid;//sender's uid
      union k_sigval sigval;
    } rt;

    //SIGCHLD
    struct{
      k_i pid;//which child
      k_u uid;//sender's uid
      k_i status;//exit code
      k_ll utime __attribute__((aligned(4));
      k_ll stime __attribute__((aligned(4));
    } sigchld;

    //SIGILL, SIGFPE, SIGSEGV, SIGBUS
    struct{
      void* addr;//faulting insn/memory ref.
      k_s addr_lsb;//lsb of the reported address
    } sigfault;

    //SIGPOLL
    struct{
      k_l band;//POLL_IN, POLL_OUT, POLL_MSG
      k_i fd;
    } sigpoll;
  } fields;
};
#endif
