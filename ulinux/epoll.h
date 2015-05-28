#ifndef ULINUX_EPOLL_H
#define ULINUX_EPOLL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_EPOLL_CLOEXEC 02000000/*same than ULINUX_O_CLOEXEC*/

#define ULINUX_EPOLL_CTL_ADD 1
#define ULINUX_EPOLL_CTL_DEL 2
#define ULINUX_EPOLL_CTL_MOD 3

#define ULINUX_EPOLLIN      0x0001
#define ULINUX_EPOLLPRI     0x0002
#define ULINUX_EPOLLOUT     0x0004
#define ULINUX_EPOLLERR     0x0008
#define ULINUX_EPOLLHUP     0x0010
#define ULINUX_EPOLLNVAL    0x0020
#define ULINUX_EPOLLRDHUP   0x2000
#define ULINUX_EPOLLONESHOT (1<<30)
#define ULINUX_EPOLLET      (1<<31)

union ulinux_epoll_data{
  void  *ptr;
  ulinux_si  fd;
  ulinux_u32 _32;
  ulinux_u64 _64;
};

#include <ulinux/arch/epoll.h>
#endif
