#ifndef ULINUX_ATOMIC
#define ULINUX_ATOMIC
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*atomic stuff (will probably move in arch.h, that's for x86-64)*/
struct ulinux_atomic_u8{
  ulinux_u8 val;
};

static inline ulinux_u8 ulinux_atomic_u8_read(const struct ulinux_atomic_u8 *v)
{
  return (*(volatile ulinux_u8*)&(v)->val);
}
static inline void ulinux_atomic_u8_set(struct ulinux_atomic_u8 *v,ulinux_u8 u)
{
  v->val=u;
}
/*----------------------------------------------------------------------------*/
#endif
