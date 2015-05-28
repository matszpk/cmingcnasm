#ifndef ULINUX_SOCKET_IN_H
#define ULINUX_SOCKET_IN_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
struct ulinux_in_addr{ulinux_u32 s_addr;};

struct ulinux_sockaddr_in{
  ulinux_us sin_family;/*address family*/
  ulinux_u16 sin_port;/*port number*/
  struct ulinux_in_addr sin_addr;/*internet address*/

  /*pad to size of `struct sockaddr'*/
  ulinux_u8 __pad[ULINUX_SOCK_ADDR_SZ-sizeof(ulinux_ss)-sizeof(ulinux_us)
                                          -sizeof(struct ulinux_in_addr)];
};
#define ULINUX_INADDR_ANY ((ulinux_ul)0x00000000)
#endif
