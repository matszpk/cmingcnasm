/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com
*******************************************************************************/
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>

#include <ulinux/utils/ascii/block/conv/decimal/decimal.h>
#include <ulinux/utils/endian.h>

#define ulinux_loop while(1)
/*
strict ipv4 (xxx.xxx.xxx.xxx) decimal ascii block to 32 bits big endian (network endian) ipv4
C version
caller must provide a valid memory block
inplace conversion: ok because minimal address is 7 bytes to 4 bytes
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_2ipv4_blk(ulinux_u32 *dest,ulinux_u8 *start,ulinux_u8 *end)
{/*do *not* trust content*/
  ulinux_u32 ipv4=0;
  ulinux_u8 *n_start=start;
  ulinux_u8 *n_end=start;
  ulinux_u8 u;
  ulinux_u8 i;

  i=0;
  for(ulinux_u8 i=0;i<=2;++i){/*only for the first 3 components*/
  ulinux_loop{
    if(i>2) break;

    if(*n_end=='.') return 0;

    while(n_end<=end&&(n_end-n_start)<=3&&*n_end!='.') ++n_end; 

    if(n_end==end||*n_end!='.') return 0;

    u=0;
    if(!ulinux_dec2u8_blk(&u,n_start,n_end-1)) return 0;
    ipv4|=u<<(24-(i*8)); 

    n_start=n_end+1;
    n_end=n_start;

    ++i;
  }

  u=0;
  if(!ulinux_dec2u8_blk(&u,n_start,end)) return 0;
  ipv4|=u;
  *dest=ulinux_cpu2be32(ipv4);
  return 1;
}
