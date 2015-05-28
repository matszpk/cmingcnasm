#ifndef ULINUX_WAIT_H
#define ULINUX_WAIT_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_WNOHANG    0x00000001
#define ULINUX_WUNTRACED  0x00000002
#define ULINUX_WSTOPPED   ULINUX_WUNTRACED
#define ULINUX_WEXITED    0x00000004
#define ULINUX_WCONTINUED 0x00000008
#define ULINUX_WNOWAIT    0x01000000/*don't reap, just poll status.*/

#define ULINUX_WNOTHREAD  0x20000000/*don't wait on children of other threads in
                                      this group*/
#define ULINUX_WALL       0x40000000/*wait on all children, regardless of type*/
#define ULINUX_WCLONE     0x80000000/*wait only on non-SIGCHLD children*/

/*first argument to waitid*/
#define ULINUX_P_ALL  0
#define ULINUX_P_PID  1
#define ULINUX_P_PGID 2
#endif
