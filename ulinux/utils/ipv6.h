#ifndef ULINUX_UTILS_IPV6
#define ULINUX_UTILS_IPV6
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#include <ulinux/utils/ipv6.h>
static inline ulinux_u8 ulinux_ipv6_addr_any(struct ulinux_in6_addr *a)
{
  return ((a->s6_addr32[0]|a->s6_addr32[1]| 
           a->s6_addr32[2]|a->s6_addr32[3])==0); 
}
#endif
