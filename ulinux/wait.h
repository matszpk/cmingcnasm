#ifndef ULINUX_WAIT_H
#define ULINUX_WAIT_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_WNOHANG    0x00000001
#define K_WUNTRACED  0x00000002
#define K_WSTOPPED   K_WUNTRACED
#define K_WEXITED    0x00000004
#define K_WCONTINUED 0x00000008
#define K_WNOWAIT    0x01000000//don't reap, just poll status.

#define K_WNOTHREAD  0x20000000//don't wait on children of other threads in this
                               //group
#define K_WALL       0x40000000//wait on all children, regardless of type
#define K_WCLONE     0x80000000//wait only on non-SIGCHLD children

//first argument to waitid
#define K_P_ALL  0
#define K_P_PID  1
#define K_P_PGID 2
#endif
