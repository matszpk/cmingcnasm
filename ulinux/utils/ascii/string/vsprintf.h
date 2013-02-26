#ifndef ULINUX_UTILS_ASCII_STRING_VSPRINTF_H
#define ULINUX_UTILS_ASCII_STRING_VSPRINTF_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
extern k_l u_a_vsnprintf(k_u8 *buf,k_ul sz,k_u8 *fmt,va_list args);
extern k_l u_a_snprintf(k_u8 *buf,k_ul sz,k_u8 *fmt,...);
extern k_i u_a_dprintf(k_i f,k_u8 *buf,k_ul sz,k_u8 *fmt,...);
#endif
