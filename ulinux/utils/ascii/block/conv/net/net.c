//**********************************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//**********************************************************************************************
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>

#include <ulinux/utils/ascii/block/conv/decimal/decimal.h>
#include <ulinux/utils/endian.h>

//strict ipv4 (xxx.xxx.xxx.xxx) decimal ascii block to 32 bits big endian (network endian) ipv4
//C version
//caller must provide a valid memory block
//inplace conversion: ok because minimal address is 7 bytes to 4 bytes
//return compiler boolean:success or failed
k_ut u_a_strict_2ipv4_blk(k_u32 *dest,k_u8 *start,k_u8 *end)
{//do *not* trust content
  k_u32 ipv4=0;
  k_u8 *n_start=start;
  k_u8 *n_end=start;
  k_u8 u;

  for(k_i i=0;i<=2;++i){//only for the first 3 components
    if(*n_end=='.') return 0;

    while(n_end<=end&&(n_end-n_start)<=3&&*n_end!='.') ++n_end; 

    if(n_end==end||*n_end!='.') return 0;

    u=0;
    if(!u_a_strict_dec2u8_blk(&u,n_start,n_end-1)) return 0;
    ipv4|=u<<(24-(i*8)); 

    n_start=n_end+1;
    n_end=n_start;
  }

  u=0;
  if(!u_a_strict_dec2u8_blk(&u,n_start,end)) return 0;
  ipv4|=u;
  *dest=u_cpu2be32(ipv4);
  return 1;
}
