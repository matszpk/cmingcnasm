#ifndef ULINUX_ARCH_UTILS_ENDIAN_H
#define ULINUX_ARCH_UTILS_ENDIAN_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
static inline ulinux_u16 ulinux_cpu2be16(ulinux_u16 v)
{
  return (ulinux_u16)((((ulinux_u16)(v)&(ulinux_u16)0x00ffU)<<8)
                                   |(((ulinux_u16)(v)&(ulinux_u16)0xff00U)>>8));
}
#define ulinux_be162cpu(v) ulinux_cpu2be16(v)

static inline ulinux_u32 ulinux_cpu2be32(ulinux_u32 v)
{
  asm("bswapl %0":"=r" (v):"0" (v));
  return v;
}
#define ulinux_be322cpu(v) ulinux_cpu2be32(v)

static inline ulinux_u64 ulinux_cpu2be64(ulinux_u64 val)
{
  union{
    struct{
      ulinux_u32 a;
      ulinux_u32 b;
    } s;
    ulinux_u64 u;
  } v;
  v.u=val;
  asm("bswapl %0 ; bswapl %1 ; xchgl %0,%1"
    : "=r" (v.s.a), "=r" (v.s.b)
    : "0" (v.s.a), "1" (v.s.b));
  return v.u;
}
#define ulinux_be642cpu(v) ulinux_cpu2be64(v)

/*little endian*/
#define ulinux_cpu2le32(v) (v)
#define ulinux_cpu2le64(v) (v)
#define ulinux_le322cpu(v) (v)
#define ulinux_le642cpu(v) (v)
#endif
