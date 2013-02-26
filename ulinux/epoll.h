#ifndef ULINUX_EPOLL_H
#define ULINUX_EPOLL_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_EPOLL_CLOEXEC 02000000//same than K_O_CLOEXEC

#define K_EPOLL_CTL_ADD 1
#define K_EPOLL_CTL_DEL 2
#define K_EPOLL_CTL_MOD 3

#define K_EPOLLIN      0x0001
#define K_EPOLLPRI     0x0002
#define K_EPOLLOUT     0x0004
#define K_EPOLLERR     0x0008
#define K_EPOLLHUP     0x0010
#define K_EPOLLNVAL    0x0020
#define K_EPOLLRDHUP   0x2000
#define K_EPOLLONESHOT (1<<30)
#define K_EPOLLET      (1<<31)

union k_epoll_data{
  void  *ptr;
  k_i   fd;
  k_u32 u32;
  k_u64 u64;
};

#include <ulinux/arch/epoll.h>
#endif
