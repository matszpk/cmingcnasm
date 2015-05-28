#ifndef ULINUX_IPV6_H
#define ULINUX_IPV6_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
struct ulinux_in6_addr{
  union{
    ulinux_u8  u6_addr8[16];
    ulinux_u16 u6_addr16[8];
    ulinux_u32 u6_addr32[4];
  } in6_u;
#define s6_addr	  in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 in6_u.u6_addr32
};

static inline ulinux_u8 ulinux_ipv6_addr_loopback(struct ulinux_in6_addr *a)
{
  return ((a->s6_addr32[0]|a->s6_addr32[1]|
           a->s6_addr32[2]|(a->s6_addr32[3]^ulinux_cpu2be32(1)))==0);
}

static inline ulinux_u8 ulinux_ipv6_addr_v4mapped(struct ulinux_in6_addr *a)
{
  return ((a->s6_addr32[0]|a->s6_addr32[1]|
          (a->s6_addr32[2]^ulinux_cpu2be32(0x0000ffff)))==0);
}

static inline ulinux_u8 ulinux_ipv6_addr_is_isatap(struct ulinux_in6_addr *addr)
{
  return ((addr->s6_addr32[2]|ulinux_cpu2be32(0x02000000))
                                                 ==ulinux_cpu2be32(0x02005EFE));
}
#endif
