#ifndef ULINUX_SIGNAL_SIGNAL_H
#define ULINUX_SIGNAL_SIGNAL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#include <ulinux/arch/signal/signal.h>
/*
In POSIX a signal is sent either to a specific thread (Linux task)
or to the process as a whole (Linux thread group).  How the signal
is sent determines whether it's to one thread or the whole group,
which determines which signal mask(s) are involved in blocking it
from being delivered until later.  When the signal is delivered,
either it's caught or ignored by a user handler or it has a default
effect that applies to the whole thread group (POSIX process).

The possible effects an unblocked signal set to SIG_DFL can have are:
  ignore	- Nothing Happens
  terminate	- kill the process, i.e. all threads in the group,
		  similar to exit_group.  The group leader (only) reports
     	  WIFSIGNALED status to its parent.
  coredump	- write a core dump file describing all threads using
     	  the same mm and then kill all those threads
  stop 	- stop all the threads in the group, i.e. TASK_STOPPED state

SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.
Other signals when not blocked and set to SIG_DFL behaves as follows.
The job control signals also have other special effects.

     +--------------------+------------------+
     |  POSIX signal      |  default action  |
     +--------------------+------------------+
     |  SIGHUP            |   terminate      |
     |  SIGINT            |	terminate      |
     |  SIGQUIT           |	coredump       |
     |  SIGILL            |	coredump       |
     |  SIGTRAP           |	coredump       |
     |  SIGABRT/SIGIOT    |	coredump       |
     |  SIGBUS            |	coredump       |
     |  SIGFPE            |	coredump       |
     |  SIGKILL           |	terminate(+)   |
     |  SIGUSR1           |	terminate      |
     |  SIGSEGV           |	coredump       |
     |  SIGUSR2           |	terminate      |
     |  SIGPIPE           |	terminate      |
     |  SIGALRM           |	terminate      |
     |  SIGTERM           |	terminate      |
     |  SIGCHLD           |	ignore         |
     |  SIGCONT           |	ignore(*)      |
     |  SIGSTOP           |	stop(*)(+)     |
     |  SIGTSTP           |	stop(*)        |
     |  SIGTTIN           |	stop(*)        |
     |  SIGTTOU           |	stop(*)        |
     |  SIGURG            |	ignore         |
     |  SIGXCPU           |	coredump       |
     |  SIGXFSZ           |	coredump       |
     |  SIGVTALRM         |	terminate      |
     |  SIGPROF           |	terminate      |
     |  SIGPOLL/SIGIO     |	terminate      |
     |  SIGSYS/SIGUNUSED  |	coredump       |
     |  SIGSTKFLT         |	terminate      |
     |  SIGWINCH          |	ignore         |
     |  SIGPWR            |	terminate      |
     |  SIGRTMIN-SIGRTMAX |	terminate      |
     +--------------------+------------------+
     |  non-POSIX signal  |   default action |
     +--------------------+------------------+
     |  SIGEMT            |   coredump       |
     +--------------------+------------------+

(+) For SIGKILL and SIGSTOP the action is "always", not just "default".
(*) Special job control effects:
When SIGCONT is sent, it resumes the process (all threads in the group)
from TASK_STOPPED state and also clears any pending/queued stop signals
(any of those marked with "stop(*)").  This happens regardless of blocking,
catching, or ignoring SIGCONT.  When any stop signal is sent, it clears
any pending/queued SIGCONT signals; this happens regardless of blocking,
catching, or ignored the stop signal, though (except for SIGSTOP) the
default action of stopping the process may happen later or never.
*/

#define ULINUX_SFD_NONBLOCK ULINUX_O_NONBLOCK
/*----------------------------------------------------------------------------*/
/*user siginfo stuff*/
struct ulinux_signalfd_siginfo{
  ulinux_u32 ssi_signo;
  ulinux_s32 ssi_errno;
  ulinux_s32 ssi_code;
  ulinux_u32 ssi_pid;
  ulinux_u32 ssi_uid;
  ulinux_s32 ssi_fd;
  ulinux_u32 ssi_tid;
  ulinux_u32 ssi_band;
  ulinux_u32 ssi_overrun;
  ulinux_u32 ssi_trapno;
  ulinux_s32 ssi_status;
  ulinux_s32 ssi_int;
  ulinux_u64 ssi_ptr;
  ulinux_u64 ssi_utime;
  ulinux_u64 ssi_stime;
  ulinux_u64 ssi_addr;
  ulinux_u16 ssi_addr_lsb;

  /*Pad strcture to 128 bytes. Remember to update the
    pad size when you add new members. We use a fixed
    size structure to avoid compatibility problems with
    future versions, and we leave extra space for additional
    members. We use fixed size members because this strcture
    comes out of a read(2) and we really don't want to have
    a compat on read(2).*/
  ulinux_u8 __pad[46];
};
/*----------------------------------------------------------------------------*/
#endif
