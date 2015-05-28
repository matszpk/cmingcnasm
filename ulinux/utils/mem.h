#ifndef ULINUX_UTILS_MEM_H
#define ULINUX_UTILS_MEM_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
void ulinux_memcpy(ulinux_u8 *d,ulinux_u8 *s,ulinux_u64 len);
void ulinux_memset(ulinux_u8 *d,ulinux_u8 c,ulinux_u64 len);
ulinux_s8 ulinux_memcmp(ulinux_u8 *s1,ulinux_u8 *s2,ulinux_u64 len);
#endif
