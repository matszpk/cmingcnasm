//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <stddef.h>

//XXX:need a way to get the target CPUID flags in order to select the proper
//code path at compilation time and not runtime
void u_memcpy(void *d,void *s,k_ul len)
{
  __builtin_memcpy(d,s,len);//use gcc builtin for this arch
}

//a memcpy implementation is needed anyway if gcc decides not to use its builtin
void *memcpy(void *to,const void *from,size_t len)
{
  k_u8 *d=to;
  k_u8 *s=(k_u8*)from;
  while(len--) *d++=*s++;
  return to;
}

void u_memset(void *d,k_u8 c,k_ul len)
{
  __builtin_memset(d,c,len);//use gcc builtin for this arch
}

//a memset implementation is needed anyway if gcc decides not to use its builtin
void *memset(void *to,int c,size_t len)
{
  k_u8 *d=to;
  while(len--) *d++=c;
  return to;
}

k_t u_memcmp(void *d,void *c,k_ul len)
{
  return __builtin_memcmp(d,c,len);//use gcc builtin for this arch
}

//a memset implementation is needed anyway if gcc decides not to use its builtin
//stolen from linux
int memcmp(const void *cs,const void *ct,size_t count)
{
  const k_u8 *su1,*su2;
  k_i res=0;

  for(su1=cs,su2=ct;0<count;++su1,++su2,count--) if((res=*su1-*su2)!=0) break;
  return res;
}
