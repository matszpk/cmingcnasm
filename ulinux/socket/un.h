#ifndef ULINUX_SOCKET_UN_H
#define ULINUX_SOCKET_UN_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_UNIX_PATH_MAX 108

struct ulinux_sockaddr_un {
  ulinux_us sun_family;
  ulinux_u8 sun_path[ULINUX_UNIX_PATH_MAX];
};
#endif
