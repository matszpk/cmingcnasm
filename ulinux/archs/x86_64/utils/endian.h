#ifndef ULINUX_ARCH_UTILS_ENDIAN_H
#define ULINUX_ARCH_UTILS_ENDIAN_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//==============================================================================
//big endian
static inline k_u16 u_cpu2be16(k_u16 v)
{
  return (k_u16)((((k_u16)(v)&(k_u16)0x00ffU)<<8)
                                             |(((k_u16)(v)&(k_u16)0xff00U)>>8));
}
#define u_be162cpu(v) u_cpu2be16(v)

static inline k_u32 u_cpu2be32(k_u32 v)
{
  asm("bswapl %0":"=r" (v):"0" (v));
  return v;
}
#define u_be322cpu(v) u_cpu2be32(v)

static inline k_u64 u_cpu2be64(k_u64 val)
{
  union{
    struct{
      k_u32 a;
      k_u32 b;
    } s;
    k_u64 u;
  } v;
  v.u=val;
  asm("bswapl %0 ; bswapl %1 ; xchgl %0,%1"
    : "=r" (v.s.a), "=r" (v.s.b)
    : "0" (v.s.a), "1" (v.s.b));
  return v.u;
}
#define u_be642cpu(v) u_cpu2be64(v)
//==============================================================================

//==============================================================================
//little endian
#define u_cpu2le32(v) (v)
#define u_cpu2le64(v) (v)
//==============================================================================
#endif
