/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>

#include <ulinux/utils/ascii/ascii.h>

#define ulinux_loop while(1)

/*
strict unsigned hexadecimal ascii block to u16
C version
caller must provide a valid memory block
inplace conversion: ok
return compiler boolean:success or failed
*/
ulinux_u8 ulinux_hex2u16_blk(ulinux_u16 *dest,ulinux_u8 *start,ulinux_u8 *end)
{/*do *not* trust content*/
	if((ulinux_u64)(end-start)>=sizeof("ffff")-1) return 0;
	*dest=0;
	ulinux_loop{
		ulinux_u8 base;

		if(start>end) break;

		base=ulinux_is_hex(*start);
		if(!base) return 0;
		*dest=(*dest<<4)+(*start-base);
		++start;
	}
	/*no overflow*/
	return 1;
}
