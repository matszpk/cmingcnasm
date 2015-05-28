#ifndef ULINUX_FCNTL_H
#define ULINUX_FCNTL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
/*some commands may be already defined for some archs*/
#define ULINUX_F_DUPFD     0/*dup*/
#define ULINUX_F_GETFD     1/*get close_on_exec*/
#define ULINUX_F_SETFD     2/*set/clear close_on_exec*/
#define ULINUX_F_GETFL     3/*get file->f_flags*/
#define ULINUX_F_SETFL     4/*set file->f_flags*/
#ifndef ULINUX_F_GETLK
#define ULINUX_F_GETLK     5
#define ULINUX_F_SETLK     6
#define ULINUX_F_SETLKW    7
#endif
#ifndef ULINUX_F_SETOWN
#define ULINUX_F_SETOWN    8/*for sockets*/
#define ULINUX_F_GETOWN    9/*for sockets*/
#endif
#ifndef ULINUX_F_SETSIG
#define ULINUX_F_SETSIG    10/*for sockets*/
#define ULINUX_F_GETSIG    11/*for sockets*/
#endif
#ifndef ULINUX_F_SETOWN_EX
#define ULINUX_F_SETOWN_EX 15
#define ULINUX_F_GETOWN_EX 16
#endif
#endif
