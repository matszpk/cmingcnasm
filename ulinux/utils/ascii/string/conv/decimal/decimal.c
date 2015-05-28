/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/

#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>

#define ulinux_loop while(1)

/*
strict unsigned decimal ascii str to u8
C version
caller must provide a valid pointer with readable data
inplace conversion: ok
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_dec2u8(ulinux_u8 *dest,ulinux_u8 *start)
{/*do *not* trust*/
  ulinux_u64 len;
  if(!ulinux_strnlen(&len,start,sizeof("255")-1)) return 0;
  if(!len) return 0;

  ulinux_u16 buf=0;
  ulinux_u8 *end=start+len-1;
  ulinux_loop{
    if(start>end) break;

    if(!ulinux_is_digit(*start)) return 0;
    buf=buf*10+(*start-'0');
    ++start;
  }
  /*overflow, max is 999 way below what can do a u16*/
  if(buf&0xff00) return 0;
  *dest=(ulinux_u8)buf;
  return 1;
}

/*
strict unsigned decimal ascii str to u16
C version
caller must provide a valid pointer with readable data
inplace conversion: ok
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_dec2u16(ulinux_u16 *dest,ulinux_u8 *start)
{/*do *not* trust*/
  ulinux_u64 len;
  if(!ulinux_strnlen(&len,start,sizeof("65535")-1)) return 0;
  if(!len) return 0;

  ulinux_u32 buf=0;
  ulinux_u8 *end=start+len-1;
  ulinux_loop{
    if(start>end) break;

    if(!ulinux_is_digit(*start)) return 0;
    buf=buf*10+(*start-'0');
    ++start;
  }
  /*overflow, max is 99999 way below what can do a u32*/
  if(buf&0xffff0000) return 0;
  *dest=(ulinux_u16)buf;
  return 1;
}

/*
strict unsigned decimal ascii str to u32
C version
caller must provide a valid pointer with readable data
inplace conversion: ok
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_dec2u32(ulinux_u32 *dest,ulinux_u8 *start)
{/*do *not* trust*/
  ulinux_u64 len;
  if(!ulinux_strnlen(&len,start,sizeof("4294967295")-1)) return 0;
  if(!len) return 0;

  ulinux_u64 buf=0;
  ulinux_u8 *end=start+len-1;
  ulinux_loop{
    if(start>end) break;
    if(!ulinux_is_digit(*start)) return 0;
    buf=buf*10+(*start-'0');
    ++start;
  }
  /*overflow, max is 9999999999 way below what can do a u32*/
  if(buf&0xffffffff00000000) return 0;
  *dest=(ulinux_u32)buf;
  return 1;
}

/*
strict unsigned decimal ascii str to u64
C version
caller must provide a valid pointer with readable data
inplace conversion: ok
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_dec2u64(ulinux_u64 *dest,ulinux_u8 *start)
{/*do *not* trust content*/
  ulinux_u64 len;
  if(!ulinux_strnlen(&len,start,sizeof("18446744073709551615")-1)) return 0;
  if(!len) return 0;

  ulinux_u64 buf=0;
  ulinux_u8 *end=start+len-1;
  ulinux_loop{
    if(start>end) break;

    if(!ulinux_is_digit(*start)) return 0;
    ulinux_u64 old_buf=buf;
    buf=buf*10+(*start-'0');
    if(buf<old_buf) return 0;/*overflow check*/
    ++start;
  }
  *dest=buf;
  return 1;
}
