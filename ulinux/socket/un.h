#ifndef ULINUX_SOCKET_UN_H
#define ULINUX_SOCKET_UN_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_UNIX_PATH_MAX 108

struct k_sockaddr_un {
  k_us sun_family;
  k_u8 sun_path[K_UNIX_PATH_MAX];
};
#endif
