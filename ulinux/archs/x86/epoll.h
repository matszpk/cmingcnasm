#ifndef ULINUX_ARCH_EPOLL_H
#define ULINUX_ARCH_EPOLL_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct k_epoll_event{
  k_u32 events;
  union k_epoll_data data;
};
#endif
