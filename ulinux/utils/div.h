#ifndef ULINUX_UTILS_DIV_H
#define ULINUX_UTILS_DIV_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define u_do_div(n,base)({     \
  k_u32 __base=(base);       \
  k_u32 __rem;               \
  __rem=((k_u64)(n))%__base; \
  (n)=((k_u64)(n))/__base;   \
  __rem;                     \
})
#endif
