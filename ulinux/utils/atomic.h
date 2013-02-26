#ifndef ULINUX_ATOMIC
#define ULINUX_ATOMIC
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//------------------------------------------------------------------------------
//|atomic stuff (will probably move in arch.h, that's for x86-64)
//------------------------------------------------------------------------------
typedef struct{
  k_u8 val;
} k_atomic_u8_t;

static inline k_u8 k_atomic_u8_read(const k_atomic_u8_t *v)
{
  return (*(volatile k_u8*)&(v)->val);
}
static inline void k_atomic_u8_set(k_atomic_u8_t *v,k_u8 u)
{
  v->val=u;
}
//------------------------------------------------------------------------------
#endif
