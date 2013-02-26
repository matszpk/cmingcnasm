#ifndef ULINUX_UTILS_MEM_H
#define ULINUX_UTILS_MEM_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
void u_memcpy(void *d,void *s,k_ul len);
void u_memset(void *d,k_u8 c,k_ul len);
k_t u_memcmp(void *s1,void *s2,k_ul len);
#endif
