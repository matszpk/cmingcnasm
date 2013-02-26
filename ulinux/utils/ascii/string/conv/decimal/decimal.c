//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************

#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>

//strict unsigned decimal ascii str to u16
//C version
//caller must provide a valid pointer with readable data
//inplace conversion: ok
//return compiler boolean:success or failed
k_ut u_a_strict_dec2u16(k_u16 *dest,k_u8 *start)
{//do *not* trust
  k_ul len;
  if(!u_a_strnlen(&len,start,sizeof("65535")-1)) return 0;
  if(!len) return 0;

  k_u32 buf=0;
  k_u8 *end=start+len-1;
  while(start<=end){
    if(!u_a_is_digit(*start)) return 0;
    buf=buf*10+(*start-'0');
    ++start;
  }
  //overflow, max is 99999 way below what can do a u32
  if(buf&0xffff0000) return 0;
  *dest=(k_u16)buf;
  return 1;
}

//strict unsigned decimal ascii str to u32
//C version
//caller must provide a valid pointer with readable data
//inplace conversion: ok
//return compiler boolean:success or failed
k_ut u_a_strict_dec2u32(k_u32 *dest,k_u8 *start)
{//do *not* trust
  k_ul len;
  if(!u_a_strnlen(&len,start,sizeof("4294967295")-1)) return 0;
  if(!len) return 0;

  k_u64 buf=0;
  k_u8 *end=start+len-1;
  while(start<=end){
    if(!u_a_is_digit(*start)) return 0;
    buf=buf*10+(*start-'0');
    ++start;
  }
  //overflow, max is 9999999999 way below what can do a u32
  if(buf&0xffffffff00000000) return 0;
  *dest=(k_u32)buf;
  return 1;
}

//strict unsigned decimal ascii str to u64
//C version
//caller must provide a valid pointer with readable data
//inplace conversion: ok
//return compiler boolean:success or failed
k_ut u_a_strict_dec2u64(k_u64 *dest,k_u8 *start)
{//do *not* trust content
  k_ul len;
  if(!u_a_strnlen(&len,start,sizeof("18446744073709551615")-1)) return 0;
  if(!len) return 0;

  k_u64 buf=0;
  k_u8 *end=start+len-1;
  while(start<=end){
    if(!u_a_is_digit(*start)) return 0;
    k_u64 old_buf=buf;
    buf=buf*10+(*start-'0');
    if(buf<old_buf) return 0;//overflow check
    ++start;
  }
  *dest=buf;
  return 1;
}
