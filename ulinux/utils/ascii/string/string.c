//**********************************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//**********************************************************************************************
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>

k_ut u_a_strnlen(k_ul *sz,k_u8 *start,k_ul max)
{
  k_u8 *end;
  for(end=start;max--&&*end!=0;++end);
  if(*end==0){
    *sz=end-start;
    return 1;
  }
  return 0;
}
