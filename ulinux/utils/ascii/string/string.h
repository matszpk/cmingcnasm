#ifndef ULINUX_UTILS_ASCII_STRING_STRING_H
#define ULINUX_UTILS_ASCII_STRING_STRING_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//strncmp - Compare two length-limited strings
//@cs: One string
//@ct: Another string
//@n: The maximum number of bytes to compare
static inline k_i u_a_strncmp(k_u8 *cs,k_u8 *ct,k_ul n)
{
  k_u8 c1,c2;
  while(n){
    c1=*cs++;
    c2=*ct++;
    if(c1!=c2) return c1<c2?-1:1;
    if(!c1) break;
    n--;
  }
  return 0;
}

static inline void u_a_strncpy(k_u8 *d,k_u8 *s,k_ul n)
{
  while(n&&*s){
    *d++=*s++;
    n--;
  }
  while(n){
    *d++=0;
    n--;
  }
}

extern k_ut u_a_strnlen(k_ul *sz,k_u8 *start,k_ul max);
#endif
