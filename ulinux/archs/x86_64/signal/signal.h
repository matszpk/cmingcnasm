#ifndef ULINUX_ARCH_SIGNAL_SIGNAL_H
#define ULINUX_ARCH_SIGNAL_SIGNAL_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_SIG_BLOCK   0//for blocking signals
#define K_SIG_UNBLOCK 1//for unblocking signals
#define K_SIG_SETMASK 2//for setting the signal mask

#define K_SIGHUP     1
#define K_SIGINT     2
#define K_SIGQUIT    3
#define K_SIGILL     4
#define K_SIGTRAP    5
#define K_SIGABRT    6
#define K_SIGIOT     6
#define K_SIGBUS     7
#define K_SIGFPE     8
#define K_SIGKILL    9
#define K_SIGUSR1   10
#define K_SIGSEGV   11
#define K_SIGUSR2   12
#define K_SIGPIPE   13
#define K_SIGALRM   14
#define K_SIGTERM   15
#define K_SIGSTKFLT 16
#define K_SIGCHLD   17
#define K_SIGCONT   18
#define K_SIGSTOP   19
#define K_SIGTSTP   20
#define K_SIGTTIN   21
#define K_SIGTTOU   22
#define K_SIGURG    23
#define K_SIGXCPU   24
#define K_SIGXFSZ   25
#define K_SIGVTALRM 26
#define K_SIGPROF   27
#define K_SIGWINCH  28
#define K_SIGIO     29
#define K_SIGPOLL   K_SIGIO
#define K_SIGPWR    30
#define K_SIGSYS    31
#define	K_SIGUNUSED 31
#define K_SIGRTMIN  32
#define K_SIGRTMAX  64
#endif
