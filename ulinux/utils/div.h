#ifndef ULINUX_UTILS_DIV_H
#define ULINUX_UTILS_DIV_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ulinux_do_div(n,base)({     \
  ulinux_u32 __base=(base);       \
  ulinux_u32 __rem;               \
  __rem=((ulinux_u64)(n))%__base; \
  (n)=((ulinux_u64)(n))/__base;   \
  __rem;                     \
})
#endif
