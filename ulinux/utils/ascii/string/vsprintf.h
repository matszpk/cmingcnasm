#ifndef ULINUX_UTILS_ASCII_STRING_VSPRINTF_H
#define ULINUX_UTILS_ASCII_STRING_VSPRINTF_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
extern ulinux_u64 ulinux_vsnprintf(ulinux_u8 *buf,ulinux_u64 sz,ulinux_u8 *fmt,
                                                                  va_list args);
extern ulinux_u64 ulinux_snprintf(ulinux_u8 *buf,ulinux_u64 sz,ulinux_u8 *fmt,
                                                                           ...);
extern ulinux_s8 ulinux_dprintf(ulinux_si f,ulinux_u8 *buf,ulinux_u64 sz,
                                                            ulinux_u8 *fmt,...);
#endif
