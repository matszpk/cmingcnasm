//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/utils/ascii/ascii.h>
//strict unsigned hexadecimal ascii block to u16
//C version
//caller must provide a valid memory block
//inplace conversion: ok
//return compiler boolean:success or failed
k_ut u_a_strict_hex2u16_blk(k_u16 *dest,k_u8 *start,k_u8 *end)
{//do *not* trust content
  if((k_ul)(end-start)>=sizeof("ffff")-1) return 0;
  *dest=0;
  while(start<=end){
    k_u8 base=u_a_is_hex(*start);
    if(!base) return 0;
    *dest=(*dest<<4)+(*start-base);
    ++start;
  }
  //no overflow
  return 1;
}

