#ifndef ULINUX_ARCH_UTILS_ENDIAN_H
#define ULINUX_ARCH_UTILS_ENDIAN_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
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

static inline k_u64 u_cpu2be64(k_u64 v)
{
  asm("bswapq %0":"=r" (v):"0" (v));
  return v;
}
#define u_be642cpu(v) u_cpu2be64(v)
#endif
