/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <stddef.h>

/*
XXX:need a way to get the target CPUID flags in order to select the proper
code path at compilation time and not runtime
*/
void ulinux_memcpy(ulinux_u8 *d,ulinux_u8 *s,ulinux_u64 len)
{
  __builtin_memcpy(d,s,(size_t)len);/*use gcc builtin for this arch*/
}

/*
a memcpy implementation is needed anyway if gcc decides not to use its builtin
*/
void *memcpy(void *to,const void *from,size_t len)
{
  ulinux_u8 *d=to;
  ulinux_u8 *s=(ulinux_u8*)from;
  while(len--) *d++=*s++;
  return to;
}

void ulinux_memset(ulinux_u8 *d,ulinux_u8 c,ulinux_u64 len)
{
  __builtin_memset(d,c,(size_t)len);/*use gcc builtin for this arch*/
}

/*
a memset implementation is needed anyway if gcc decides not to use its builtin
*/
void *memset(void *to,int c,size_t len)
{
  ulinux_u8 *d=to;
  while(len--) *d++=c;
  return to;
}

ulinux_s8 ulinux_memcmp(ulinux_u8 *d,ulinux_u8 *c,ulinux_u64 len)
{
  return __builtin_memcmp(d,c,(size_t)len);/*use gcc builtin for this arch*/
}

/*
a memset implementation is needed anyway if gcc decides not to use its builtin
stolen from linux
*/
int memcmp(const void *cs,const void *ct,size_t count)
{
  const ulinux_u8 *su1,*su2;
  ulinux_si res=0;

  for(su1=cs,su2=ct;0<count;++su1,++su2,count--) if((res=*su1-*su2)!=0) break;
  return res;
}
