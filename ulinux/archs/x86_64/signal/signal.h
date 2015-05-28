#ifndef ULINUX_ARCH_SIGNAL_SIGNAL_H
#define ULINUX_ARCH_SIGNAL_SIGNAL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_SIG_BLOCK   0/*for blocking signals*/
#define ULINUX_SIG_UNBLOCK 1/*for unblocking signals*/
#define ULINUX_SIG_SETMASK 2/*for setting the signal mask*/

#define ULINUX_SIGHUP     1
#define ULINUX_SIGINT     2
#define ULINUX_SIGQUIT    3
#define ULINUX_SIGILL     4
#define ULINUX_SIGTRAP    5
#define ULINUX_SIGABRT    6
#define ULINUX_SIGIOT     6
#define ULINUX_SIGBUS     7
#define ULINUX_SIGFPE     8
#define ULINUX_SIGKILL    9
#define ULINUX_SIGUSR1   10
#define ULINUX_SIGSEGV   11
#define ULINUX_SIGUSR2   12
#define ULINUX_SIGPIPE   13
#define ULINUX_SIGALRM   14
#define ULINUX_SIGTERM   15
#define ULINUX_SIGSTKFLT 16
#define ULINUX_SIGCHLD   17
#define ULINUX_SIGCONT   18
#define ULINUX_SIGSTOP   19
#define ULINUX_SIGTSTP   20
#define ULINUX_SIGTTIN   21
#define ULINUX_SIGTTOU   22
#define ULINUX_SIGURG    23
#define ULINUX_SIGXCPU   24
#define ULINUX_SIGXFSZ   25
#define ULINUX_SIGVTALRM 26
#define ULINUX_SIGPROF   27
#define ULINUX_SIGWINCH  28
#define ULINUX_SIGIO     29
#define ULINUX_SIGPOLL   K_SIGIO
#define ULINUX_SIGPWR    30
#define ULINUX_SIGSYS    31
#define	ULINUX_SIGUNUSED 31
#define ULINUX_SIGRTMIN  32
#define ULINUX_SIGRTMAX  64
#endif
