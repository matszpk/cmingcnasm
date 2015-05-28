#ifndef ULINUX_FILE_H
#define ULINUX_FILE_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ULINUX_O_ACCMODE   00000003
#define ULINUX_O_RDONLY    00000000
#define ULINUX_O_WRONLY    00000001
#define ULINUX_O_RDWR      00000002
#define ULINUX_O_CREAT     00000100/*not fcntl*/
#define ULINUX_O_EXCL      00000200/*not fcntl*/
#define ULINUX_O_NOCTTY    00000400/*not fcntl*/
#define ULINUX_O_TRUNC     00001000/*not fcntl*/
#define ULINUX_O_APPEND    00002000
#define ULINUX_O_NONBLOCK  00004000
#define ULINUX_O_DSYNC     00010000/*used to be O_SYNC, see below*/
#define ULINUX_FASYNC      00020000/*fcntl, for BSD compatibility*/
#define ULINUX_O_DIRECT    00040000/*direct disk access hint*/
#define ULINUX_O_LARGEFILE 00100000
#define ULINUX_O_DIRECTORY 00200000/*must be a directory*/
#define ULINUX_O_NOFOLLOW  00400000/*don't follow links*/
#define ULINUX_O_NOATIME   01000000
#define ULINUX_O_CLOEXEC   02000000/*set close_on_exec*/

#define __ULINUX_O_SYNC    04000000
#define ULINUX_O_SYNC      (__ULINUX_O_SYNC|ULINUX_O_DSYNC)
#endif
