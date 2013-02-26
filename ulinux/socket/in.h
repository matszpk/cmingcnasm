#ifndef ULINUX_SOCKET_IN_H
#define ULINUX_SOCKET_IN_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct k_in_addr{k_u32 s_addr;};

struct k_sockaddr_in{
  k_us sin_family;//address family
  k_u16 sin_port;//port number
  struct k_in_addr sin_addr;//internet address

  //pad to size of `struct sockaddr'
  k_u8 __pad[__SOCK_ADDR_SZ__-sizeof(k_s)-sizeof(k_us)
                                          -sizeof(struct k_in_addr)];
};
#define K_INADDR_ANY ((k_ul)0x00000000)
#endif
