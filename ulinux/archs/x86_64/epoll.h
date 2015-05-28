#ifndef ULINUX_ARCH_EPOLL_H
#define ULINUX_ARCH_EPOLL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
struct ulinux_epoll_event{
  ulinux_u32 events;
  union ulinux_epoll_data data;
} __attribute__((packed));/*packed is specific to x86_64*/
#endif
