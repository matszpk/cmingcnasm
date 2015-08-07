/*******************************************************************************
this code is protected by the GNU affero GPLv3
ripper:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
ripped and sort of ported (heavily derived) to user space from Linux kernel
(then the linux GPLv2 does not apply).
*******************************************************************************/

/*
linux/lib/vsprintf.c

 Copyright (C) 1991, 1992  Linus Torvalds


vsprintf.c -- Lars Wirzenius & Linus Torvalds.

Wirzenius wrote this portably, Torvalds fucked it up :-)


Fri Jul 13 2001 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
- changed to provide snprintf and vsnprintf functions
So Feb  1 16:51:32 CET 2004 Juergen Quade <quade@hsnr.de>
- scnprintf and vscnprintf
*/

#include <stdarg.h>

#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/mmap.h>
#include <ulinux/utils/div.h>
#include <ulinux/utils/endian.h>
#include <ulinux/ipv6.h>
#include <ulinux/utils/ipv6.h>
#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>

#define ulinux_loop while(1)

/*works only for digits and letters, but small and fast*/
#define TOLOWER(x) ((x)|0x20)

#define ZEROPAD	1 /*pad with zero*/
#define SIGN	2 /*unsigned/signed long*/
#define PLUS	4 /*show plus*/
#define SPACE	8 /*space if plus*/
#define LEFT	16/*left justified*/
#define SMALL	32/*use lowercase in hex (must be 32 == 0x20)*/
#define SPECIAL	64/*prefix hex with "0x", octal with "0"*/

  /*just a string part*/
#define FMT_TYPE_NONE		0
#define FMT_TYPE_WIDTH		1
#define FMT_TYPE_PRECISION	2
#define FMT_TYPE_CHAR		3
#define FMT_TYPE_STR		4
#define FMT_TYPE_PTR		5
#define FMT_TYPE_PERCENT_CHAR	6
#define FMT_TYPE_INVALID	7
#define FMT_TYPE_LONG_LONG	8
#define FMT_TYPE_ULONG		9
#define FMT_TYPE_LONG		10
#define FMT_TYPE_UBYTE		11
#define FMT_TYPE_BYTE		12
#define FMT_TYPE_USHORT		13
#define FMT_TYPE_SHORT		14
#define FMT_TYPE_UINT		15
#define FMT_TYPE_INT		16
#define FMT_TYPE_NRCHARS	17
#define FMT_TYPE_SIZE_T		18
#define FMT_TYPE_PTRDIFF	19

struct printf_spec {
	ulinux_u8 type;		/*format_type enum*/
	ulinux_u8 flags;	/*flags to number()*/
	ulinux_u8 base;		/*number base, 8, 10 or 16 only*/
	ulinux_u8 qualifier;	/*number qualifier, one of 'hHlLtzZ'*/
	ulinux_s16 field_width;	/*width of output field, -1 means not present or
	                          ignored for some reason*/
	ulinux_s16 precision;	/*# of digits/chars, -1 means not present or
				  ignored for some reason*/
};

/*Same with if's removed. Always emits five digits.*/
static ulinux_u8 *put_dec_full(ulinux_u8 *buf,ulinux_u64 q)
{
	/*BTW, if q is in [0,9999], 8-bit ints will be enough,
	  but anyway, gcc produces better code with full-sized ints*/
	ulinux_u64 d3,d2,d1,d0;
	d1=(q>>4)&0xf;
	d2=(q>>8)&0xf;
	d3=(q>>12);

	/*Possible ways to approx. divide by 10
	  gcc -O2 replaces multiply with shifts and adds
	  (x * 0xcd) >> 11: 11001101 - shorter code than * 0x67 (on i386)
	  (x * 0x67) >> 10:  1100111
	  (x * 0x34) >> 9:    110100 - same
	  (x * 0x1a) >> 8:     11010 - same
	  (x * 0x0d) >> 7:      1101 - same, shortest code (on i386)*/
	d0=6*(d3+d2+d1)+(q&0xf);
	q=(d0*0xcd)>>11;
	d0=d0-10*q;
	*buf++=d0+'0';
	d1=q+9*d3+5*d2+d1;
	q=(d1*0xcd)>>11;
	d1=d1-10*q;
	*buf++=d1+'0';
	
	d2=q+2*d2;
	q=(d2*0xd)>>7;
	d2=d2-10*q;
	*buf++=d2+'0';
	
	d3=q+4*d3;
	q=(d3*0xcd)>>11;/*- shorter code*/
	/*q = (d3 * 0x67) >> 10; - would also work*/
	d3=d3-10*q;
	*buf++=d3+'0';
	*buf++=q+'0';
	return buf;
}

/*
Decimal conversion is by far the most typical, and is used
for /proc and /sys data. This directly impacts e.g. top performance
with many processes running. We optimize it for speed
using code from
http://www.cs.uiowa.edu/~jones/bcd/decimal.html
(with permission from the author, Douglas W. Jones).

Formats correctly any integer in [0,99999].
Outputs from one to five digits depending on input.
On i386 gcc 4.1.2 -O2: ~250 bytes of code.
*/
static ulinux_u8 *put_dec_trunc(ulinux_u8 *buf,ulinux_u64 q)
{
	ulinux_u64 d3,d2,d1,d0;
	d1=(q>>4)&0xf;
	d2=(q>>8)&0xf;
	d3=(q>>12);
	
	d0=6*(d3+d2+d1)+(q&0xf);
	q=(d0*0xcd)>>11;
	d0=d0-10*q;
	*buf++=d0+'0';/*least significant digit*/
	d1=q+9*d3+5*d2+d1;
	if(d1!=0){
		q=(d1*0xcd)>>11;
		d1=d1-10*q;
		*buf++=d1+'0';/*next digit*/
		
		d2=q+2*d2;
		if((d2!=0)||(d3!= 0)){
			q=(d2*0xd)>>7;
			d2=d2-10*q;
			*buf++=d2+'0';/*next digit*/
			
			d3=q+4*d3;
			if(d3!=0){
				q=(d3*0xcd)>>11;
				d3=d3-10*q;
				*buf++=d3+'0';/*next digit*/
				if(q!=0) *buf++=q+'0';/*most sign. digit*/
			}
		}
	}
	return buf;
}

/*no inlining helps gcc to use registers better*/
static ulinux_u8 *put_dec(ulinux_u8 *buf,ulinux_u64 num)
{
	ulinux_loop{
		ulinux_u64 rem;
		if(num<100000) return put_dec_trunc(buf,num);
		rem=ulinux_do_div(num,100000);
		buf=put_dec_full(buf,rem);
	}
}

static ulinux_u8 *number(ulinux_u8 *buf,ulinux_u8 *end,ulinux_u64 num,
                                                        struct printf_spec spec)
{
	ulinux_u8 locase;
	ulinux_u8 sign;
	ulinux_u8 need_pfx;
	ulinux_u8 *digits;
	/*octal max digit number for unsigned 64 bits plus sign*/
	ulinux_u8 tmp[66];
	ulinux_s64 i;

	/*we are called with base 8, 10 or 16, only, thus don't need "G..."
	  locase = 0 or 0x20. ORing digits or letters with 'locase'
	  produces same digits or (maybe lowercased) letters*/
	locase=spec.flags&SMALL;
	if(spec.flags&LEFT) spec.flags&=~ZEROPAD;

	sign=0;
	if(spec.flags&SIGN){
		if((ulinux_s64)num<0){
			sign='-';
			num=-(ulinux_s64)num;
			spec.field_width--;
		}else if(spec.flags&PLUS){
			sign='+';
			spec.field_width--;
		}else if(spec.flags&SPACE){
			sign=' ';
			spec.field_width--;
		}
	}

	need_pfx=((spec.flags&SPECIAL)&&spec.base!=10);
	if(need_pfx){
		spec.field_width--;
		if(spec.base==16) spec.field_width--;
	}

	/*generate full string in tmp[], in reverse order*/
	digits=(ulinux_u8*)"0123456789ABCDEF";/*"GHIJKLMNOPQRSTUVWXYZ";*/
	i=0;
	if(num==0) tmp[i++]='0';
	/*Generic code, for any base:
	  else do {
	  tmp[i++] = (digits[do_div(num,base)] | locase);
	  } while (num != 0);*/
	else if(spec.base!=10){/*8 or 16*/
		ulinux_u8 mask;
		ulinux_u8 shift;

		mask=spec.base-1;
		shift=3;
		
		if(spec.base==16) shift=4;

		ulinux_loop{
			tmp[i++]=(digits[((ulinux_u8)num)&mask]|locase);
			num>>=shift;

			if(num==0) break;
		}
	}else i=put_dec(tmp,num)-tmp;/*base 10*/

	/*printing 100 using %2d gives "100", not "00"*/
	if(i>spec.precision) spec.precision=i;
	/*leading space padding*/
	spec.field_width-=spec.precision;
	if(!(spec.flags&(ZEROPAD+LEFT)))
		ulinux_loop{
			if(--spec.field_width<0) break;
			if(buf<end) *buf=' ';
			++buf;
		}
	/*sign*/
	if(sign){
		if(buf<end) *buf=sign;
		++buf;
	}
	/*"0x" / "0" prefix*/
	if(need_pfx){
		if(buf<end) *buf='0';
		++buf;
		if(spec.base==16){
			if(buf<end) *buf=('X'|locase);
			++buf;
		}
	}
	/*zero or space padding*/
	if(!(spec.flags & LEFT)){
		ulinux_u8 c;

		c=(spec.flags&ZEROPAD)?'0':' ';
		
		ulinux_loop{
			if(--spec.field_width<0) break;
			if(buf<end) *buf=c;
			++buf;
		}
	}
	/*hmm even more zero padding?*/
	ulinux_loop{
		if(i>--spec.precision) break;
		if(buf<end) *buf='0';
		++buf;
	}
	/*actual digits of result*/
	ulinux_loop{
		if(--i<0) break;
		if(buf<end) *buf=tmp[i];
		++buf;
	}
	/*trailing space padding*/
	ulinux_loop{
		if(--spec.field_width<0) break;
		if(buf<end) *buf=' ';
		++buf;
	}
	return buf;
}

static ulinux_u8 *string(ulinux_u8 *buf,ulinux_u8 *end,ulinux_u8 *s,
                                                        struct printf_spec spec)
{
	ulinux_u64 i;
	ulinux_u64 len;

	/*any pointer in the first memory page is invalid*/
	if((ulinux_u64)s<ULINUX_PAGE_SZ) s=(ulinux_u8*)"(null)";

	if(!ulinux_strnlen(&len,s,spec.precision)) len=spec.precision;

	if(!(spec.flags&LEFT))/*it's RIGHT*/
		ulinux_loop{
			if((ulinux_s16)len>=spec.field_width--) break;
			if(buf<end) *buf=' ';
			++buf;
		}

	i=0;
	ulinux_loop{
		if(i++>=len) break;
		if(buf<end) *buf=*s;
		++buf;++s;
	}

	ulinux_loop{
		if((ulinux_s16)len>=spec.field_width--) break;
		if(buf<end) *buf=' ';
		++buf;
	}
	return buf;
}

#define hex_asc_lo(x) hex_asc[((x)&0x0f)]
#define hex_asc_hi(x) hex_asc[((x)&0xf0)>>4]

static ulinux_u8 *pack_hex_byte(ulinux_u8 *buf,ulinux_u8 byte)
{
	ulinux_u8 *hex_asc;

	hex_asc=(ulinux_u8*)"0123456789abcdef";
	*buf++=hex_asc_hi(byte);
	*buf++=hex_asc_lo(byte);
	return buf;
}

static ulinux_u8 *mac_address_string(ulinux_u8 *buf,ulinux_u8 *end,
                         ulinux_u8 *addr,struct printf_spec spec,ulinux_u8 *fmt)
{
	ulinux_u8 mac_addr[sizeof("xx:xx:xx:xx:xx:xx")];
	ulinux_u8 *p;
	ulinux_u8 separator;
	ulinux_u8 i;

	p=&mac_addr[0];
  
	if(fmt[1]=='F') separator='-';/*FDDI canonical format*/
	else separator=':';
 
	i=0; 
	ulinux_loop{
		if(i>=6) break;
		
		p=pack_hex_byte(p,addr[i]);
		if(fmt[0]=='M'&&i!=5) *p++=separator;
		++i;
	}
	*p=0;
	return string(buf,end,mac_addr,spec);
}

static ulinux_u8 *ip4_string(ulinux_u8 *p,ulinux_u8 *addr,ulinux_u8 *fmt)
{
	ulinux_s8 index,step;
	ulinux_u8 i;
	ulinux_u8 leading_zeros;

	switch(fmt[2]){
	case 'h':
#		ifdef __BIG_ENDIAN
			index=0;
			step=1;
#		else
			index=3;
			step=-1;
#		endif
		break;
	case 'l':
		index=3;
		step=-1;
		break;
	case 'n':
	case 'b':
	default:
		index=0;
		step=1;
		break;
	}

	leading_zeros=(fmt[0]=='i');
	i=0;
	ulinux_loop{
		ulinux_u8 temp[3];/*hold each IP quad in reverse order*/
		ulinux_u8 digits;

		if(i>=4) break;
	
		digits=put_dec_trunc(temp,addr[index])-temp;
		if(leading_zeros){
			if(digits<3) *p++='0';
			if(digits<2) *p++='0';
		}
		/*reverse the digits in the quad*/
		ulinux_loop{
			if(digits--==0) break;
			*p++=temp[digits];
		}
		if(i<3) *p++='.';
		index+=step;
		++i;
	}
	*p=0;
	return p;
}

static ulinux_u8 *ip6_compressed_string(ulinux_u8 *p,ulinux_u8 *addr)
{
	ulinux_u8 needcolon;
	struct ulinux_in6_addr in6;
	ulinux_u8 i;
	ulinux_u8 useIPv4;
	ulinux_u8 zerolength[8];
	ulinux_u8 range; 
	ulinux_u8 longest;
	ulinux_u8 colonpos;
  
	needcolon=0;
	ulinux_memcpy((ulinux_u8*)&in6,addr,sizeof(in6));
  
	useIPv4=ulinux_ipv6_addr_v4mapped(&in6)
					||ulinux_ipv6_addr_is_isatap(&in6);
  
	ulinux_memset(zerolength,0,sizeof(zerolength));
 
	if(useIPv4) range=6;
	else range=8;
  
	/*find position of longest 0 run*/
	i=0;
	ulinux_loop{
		ulinux_u8 j;
		if(i>=range) break; 
		
		j=i;
		ulinux_loop{
			if(j>=range) break;
			
			if(in6.s6_addr16[j]!=0) break;
			zerolength[i]++;
			++j;
		}
		++i;
	}

	longest=1;
	colonpos=-1;
	i=0;
	ulinux_loop{
		if(i>=range) break;
		
		if(zerolength[i]>longest){
			longest=zerolength[i];
			colonpos=i;
		}
		++i;
	}
  
	/*emit address*/
	i=0;
	ulinux_loop{
		ulinux_u16 word;
		ulinux_u8 hi;
		ulinux_u8 lo;
		ulinux_u8 *hex_asc;

		hex_asc=(ulinux_u8*)"0123456789abcdef";

		if(i>=range) break;
		
		if(i==colonpos){
			if(needcolon||i==0) *p++=':';
			*p++=':';
			needcolon=0;
			i+=longest-1;
			continue;
		}
		if(needcolon){
			*p++=':';
			needcolon=0;
		}
		/*hex u16 without leading 0s*/
		word=ulinux_be162cpu(in6.s6_addr16[i]);
		hi=word>>8;
		lo=word&0xff;
		if(hi){
			if(hi>0x0f) p=pack_hex_byte(p,hi);
			else *p++=hex_asc_lo(hi);
			p=pack_hex_byte(p,lo);
		}else if(lo>0x0f)
			p=pack_hex_byte(p,lo);
		else
			*p++=hex_asc_lo(lo);
		needcolon=1;
		++i;
	}
  
	if(useIPv4){
		if(needcolon) *p++=':';
		p=ip4_string(p,&in6.s6_addr[12],(ulinux_u8*)"I4");
	}
	*p=0;
	return p;
}

static ulinux_u8 *ip6_string(ulinux_u8 *p,ulinux_u8 *addr,ulinux_u8 *fmt)
{
	ulinux_u8 i;
	i=0;
	ulinux_loop{
		if(i>=8) break;
		
		p=pack_hex_byte(p,*addr++);
		p=pack_hex_byte(p,*addr++);
		if(fmt[0]=='I'&&i!=7) *p++=':';
		++i;
	}
	*p=0;
	return p;
}

static ulinux_u8 *ip6_addr_string(ulinux_u8 *buf,ulinux_u8 *end,ulinux_u8 *addr,
                                         struct printf_spec spec,ulinux_u8 *fmt)
{
	ulinux_u8 ip6_addr[
		sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255")];
	
	if(fmt[0]=='I'&&fmt[2]=='c') ip6_compressed_string(ip6_addr,addr);
	else ip6_string(ip6_addr,addr,fmt);
	return string(buf,end,ip6_addr,spec);
}

static ulinux_u8 *ip4_addr_string(ulinux_u8 *buf,ulinux_u8 *end,ulinux_u8 *addr,
                                         struct printf_spec spec,ulinux_u8 *fmt)
{
	ulinux_u8 ip4_addr[sizeof("255.255.255.255")];

	ip4_string(ip4_addr,addr,fmt);
	return string(buf,end,ip4_addr,spec);
}

static void uuid_tbls_init(ulinux_u8 *be,ulinux_u8 *le)
{
	ulinux_u8 i;
	i=0;
	ulinux_loop{
		be[i]=i;
		++i;
		if(i==16) break;
	}

	le[0]=3;le[1]=2;le[2]=1;le[3]=0;le[4]=5;le[5]=4;le[6]=7;le[7]=6;
	i=8;
	ulinux_loop{
		le[i]=i;
		++i;
		if(i==16) break;
	}
}

static ulinux_u8 *uuid_string(ulinux_u8 *buf,ulinux_u8 *end,ulinux_u8 *addr,
                                         struct printf_spec spec,ulinux_u8 *fmt)
{
	ulinux_u8 be[16];
	ulinux_u8 le[16];
	ulinux_u8 *index;
	ulinux_u8 uc;
	ulinux_u8 i;
	ulinux_u8 uuid[sizeof("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx")];
	ulinux_u8 *p;
 
	uuid_tbls_init(&be[0],&le[0]); 
	index=&be[0];
	uc=0;

	switch(*(++fmt)){
	case 'L':
		uc=1;/*fall-through*/
	case 'l':
		index=le;
		break;
	case 'B':
		uc=1;
		break;
	}
  
	p=&uuid[0];
	i=0;
	ulinux_loop{
	
		if(i>=16) break;
		p=pack_hex_byte(p,addr[index[i]]);
		switch(i){
		case 3:
		case 5:
		case 7:
		case 9:
			*p++='-';
			break;
		}
		++i;
	}
	*p=0;
  
	if(uc){
		p=&uuid[0];
		ulinux_loop{
			*p=ulinux_2upper(*p);
			
			if(*(++p)==0) break;
		}
	}
	return string(buf,end,uuid,spec);
}

/*
Show a '%p' thing.  A kernel extension is that the '%p' is followed
by an extra set of alphanumeric characters that are extended format
specifiers.

Right now we handle:

- 'M' For a 6-byte MAC address, it prints the address in the
      usual colon-separated hex notation
- 'm' For a 6-byte MAC address, it prints the hex address without colons
- 'MF' For a 6-byte MAC FDDI address, it prints the address
      with a dash-separated hex notation
- 'I' [46] for IPv4/IPv6 addresses printed in the usual way
      IPv4 uses dot-separated decimal without leading 0's (1.2.3.4)
      IPv6 uses colon separated network-order 16 bit hex with leading 0's
- 'i' [46] for 'raw' IPv4/IPv6 addresses
      IPv6 omits the colons (01020304...0f)
      IPv4 uses dot-separated decimal with leading 0's (010.123.045.006)
- '[Ii]4[hnbl]' IPv4 addresses in host, network, big or little endian order
- 'I6c' for IPv6 addresses printed as specified by
      http://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-00
- 'U' For a 16 byte UUID/GUID, it prints the UUID/GUID in the form
      "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
      Options for %pU are:
        b big endian lower case hex (default)
        B big endian UPPER case hex
        l little endian lower case hex
        L little endian UPPER case hex
          big endian output byte order is:
            [0][1][2][3]-[4][5]-[6][7]-[8][9]-[10][11][12][13][14][15]
          little endian output byte order is:
            [3][2][1][0]-[5][4]-[7][6]-[8][9]-[10][11][12][13][14][15]

Note: The difference between 'S' and 'F' is that on ia64 and ppc64
function pointers are really function descriptors, which contain a
pointer to the real address.
*/
static ulinux_u8 *pointer(ulinux_u8 *fmt,ulinux_u8 *buf,ulinux_u8 *end,
                                              void *ptr,struct printf_spec spec)
{
	if(!ptr) return string(buf,end,(ulinux_u8*)"(null)",spec);

	switch(*fmt){
	case 'M':/*colon separated: 00:01:02:03:04:05*/
	case 'm':/*contiguous: 000102030405*/
		/*[mM]F (FDDI, bit reversed)*/
		return mac_address_string(buf,end,ptr,spec,fmt);
	case 'I':	/*Formatted IP supported
			  4:  1.2.3.4
			  6:  0001:0203:...:0708
			  6c: 1::708 or 1::1.2.3.4*/
	case 'i':	/*contiguous:
			  4: 001.002.003.004
			  6: 000102...0f*/
		switch(fmt[1]){
		case '6':
			return ip6_addr_string(buf,end,ptr,spec,fmt);
		case '4':
			return ip4_addr_string(buf,end,ptr,spec,fmt);
		}
		break;
	case 'U':
		return uuid_string(buf,end,ptr,spec,fmt);
	}

	spec.flags|=SMALL;
	if(spec.field_width==-1){
		spec.field_width=2*sizeof(void*);
		spec.flags|=ZEROPAD;
	}
	spec.base=16;
	return number(buf,end,(ulinux_u64)ptr,spec);
}

static inline ulinux_u16 a2u16(ulinux_u8 **s)
{
	ulinux_u16 i;

	i=0;
	ulinux_loop{
		if(!ulinux_is_digit(**s)) break;
		i=i*10+*((*s)++)-'0';
	}
	return i;
}

/*
Helper function to decode printf style format.
Each call decode a token from the format and return the
number of characters read (or likely the delta where it wants
to go on the next call).
The decoded token is returned through the parameters

'h', 'l', or 'L' for integer fields
'z' support added 23/7/1999 S.H.
'z' changed to 'Z' --davidm 1/25/99
't' added for ptrdiff_t

@fmt: the format string
@type of the token returned
@flags: various flags such as +, -, # tokens..
@field_width: overwritten width
@base: base of the number (octal, hex, ...)
@precision: precision of a number
@qualifier: qualifier of a number (long, size_t, ...)
*/
static ulinux_s64 format_decode(ulinux_u8 *fmt,struct printf_spec *spec)
{
	ulinux_u8 *start;

	start=fmt;

	/*--------------------------------------------------------------------*/
	/*we finished early by reading the field width*/
	if(spec->type==FMT_TYPE_WIDTH){
		if(spec->field_width<0){
			spec->field_width=-spec->field_width;
			spec->flags|=LEFT;
		}
		spec->type=FMT_TYPE_NONE;
		goto precision;
	}

	/*we finished early by reading the precision*/
	if(spec->type==FMT_TYPE_PRECISION){
		if(spec->precision<0) spec->precision=0;
		spec->type=FMT_TYPE_NONE;
		goto qualifier;
	}
	/*--------------------------------------------------------------------*/

	spec->type=FMT_TYPE_NONE;/*by default*/
	ulinux_loop{
		if(*fmt==0) break;
		if(*fmt=='%')break;
		++fmt;
	}

	/*return the current non-format string*/
	if(fmt!=start||!*fmt) return fmt-start;

	spec->flags=0;/*process flags*/

	ulinux_loop{/*this also skips first '%'*/
		ulinux_u8 found;

		found=1;
		++fmt;
		switch(*fmt){
		case '-':spec->flags|=LEFT;   break;
		case '+':spec->flags|=PLUS;   break;
		case ' ':spec->flags|=SPACE;  break;
		case '#':spec->flags|=SPECIAL;break;
		case '0':spec->flags|=ZEROPAD;break;
		default:found=0;
		}
		if(!found) break;
	}

	/*get field width*/
	spec->field_width=-1;

	if(ulinux_is_digit(*fmt)) spec->field_width=(ulinux_s16)a2u16(&fmt);
	else if(*fmt=='*'){
		spec->type=FMT_TYPE_WIDTH;/*get it from the argument list*/
		return ++fmt-start;
	}

	/*get the precision*/
precision:
	spec->precision=-1;
	if(*fmt=='.'){
		++fmt;
		if(ulinux_is_digit(*fmt)){
			spec->precision=(ulinux_s16)a2u16(&fmt);
			if(spec->precision<0) spec->precision=0;
		}else if(*fmt=='*'){/*it's the next argument*/
			spec->type=FMT_TYPE_PRECISION;
			return ++fmt-start;
		}else spec->precision=0;
	}

qualifier:
	/*get the conversion qualifier*/
	spec->qualifier=-1;
	if(*fmt=='h'||TOLOWER(*fmt)=='l'||TOLOWER(*fmt)=='z'||*fmt=='t'){
		spec->qualifier=*fmt++;
		if(spec->qualifier==*fmt){
			if(spec->qualifier=='l'){
				spec->qualifier='L';
				++fmt;
			}else if(spec->qualifier=='h'){
				spec->qualifier='H';
				++fmt;
			}
		}
	}

	spec->base=10;/*default base:2/8/10/16 wanted?*/
	switch(*fmt){
	case 'c':
		spec->type=FMT_TYPE_CHAR;
		return ++fmt-start;
	case 's':
		spec->type=FMT_TYPE_STR;
		return ++fmt-start;
	case 'p':
		spec->type=FMT_TYPE_PTR;
		return fmt-start;
	/*skip alnum*/
	case 'n':
		spec->type=FMT_TYPE_NRCHARS;
		return ++fmt-start;
	case '%':
		spec->type=FMT_TYPE_PERCENT_CHAR;
		return ++fmt-start;
	/*integer number formats - set up the flags and "break"*/
	case 'o':
		spec->base=8;
		break;
	case 'x':
		spec->flags|=SMALL;
	case 'X':
		spec->base=16;
		break;
	case 'd':
	case 'i':
		spec->flags|=SIGN;
	case 'u':
		break;
	default:
		spec->type=FMT_TYPE_INVALID;
		return fmt-start;
	}

	if(spec->qualifier=='L') spec->type=FMT_TYPE_LONG_LONG;
	else if(spec->qualifier=='l'){
		if(spec->flags&SIGN) spec->type=FMT_TYPE_LONG;
		else spec->type=FMT_TYPE_ULONG;
	}else if(TOLOWER(spec->qualifier)=='z'){
		spec->type=FMT_TYPE_SIZE_T;
	}else if(spec->qualifier=='t'){
		spec->type=FMT_TYPE_PTRDIFF;
	}else if(spec->qualifier=='H'){
		if(spec->flags&SIGN) spec->type=FMT_TYPE_BYTE;
		else spec->type=FMT_TYPE_UBYTE;
	}else if(spec->qualifier=='h'){
		if(spec->flags&SIGN) spec->type=FMT_TYPE_SHORT;
		else spec->type=FMT_TYPE_USHORT;
	}else{
		if(spec->flags&SIGN) spec->type=FMT_TYPE_INT;
		else spec->type=FMT_TYPE_UINT;
	}
	return ++fmt-start;
}

/*
vsnprintf - Format a string and place it in a buffer
@buf: The buffer to place the result into
@sz: The size of the buffer, including the trailing null space
@fmt: The format string to use
@args: Arguments for the format string

This function follows C99 vsnprintf, but has some extensions:
%pM output a 6-byte MAC address with colons
%pm output a 6-byte MAC address without colons
%pI4 print an IPv4 address without leading zeros
%pi4 print an IPv4 address with leading zeros
%pI6 print an IPv6 address with colons
%pi6 print an IPv6 address without colons
%pI6c print an IPv6 address as specified by
  http://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-00
%pU[bBlL] print a UUID/GUID in big or little endian using lower or upper
  case.
%n is ignored

The return value is the number of characters which would
be generated for the given input, excluding the trailing
'\0', as per ISO C99. If you want to have the exact
number of characters written into @buf as return value
(not including the trailing '\0'), use vscnprintf(). If the
return is greater than or equal to @size, the resulting
string is truncated.

Call this function if you are already dealing with a va_list.
You probably want snprintf() instead.
SECURITY:fmt is **trusted**,argument related to %fmt are **trusted**
*/
ulinux_u64 ulinux_vsnprintf(ulinux_u8 *buf,ulinux_u64 sz,ulinux_u8 *fmt,
                                                                   va_list args)
{
	ulinux_u64 num;
	struct printf_spec spec;
	ulinux_u8 *str;
	ulinux_u8 *end;
  
	str=buf;
	end=buf+sz;/*point to the byte *after* the buffer last byte*/

	spec.type=0;
	spec.flags=0;
	spec.base=0;
	spec.qualifier=0;
	spec.field_width=0;
	spec.precision=0;

	ulinux_loop{
		ulinux_u8 *old_fmt;
		ulinux_u64 read;

		if(*fmt==0) break;

		old_fmt=fmt;
		read=format_decode(fmt,&spec);

		fmt+=read;

		switch(spec.type){
		case FMT_TYPE_NONE:{
			ulinux_u64 copy;

			copy=read;
			if(str<end){
				if((ulinux_s64)copy>end-str) copy=end-str;
				ulinux_memcpy(str,old_fmt,copy);
			}
			str+=read;
			break;
		}
		case FMT_TYPE_WIDTH:/*get field width from the argument list*/
			spec.field_width=(ulinux_s16)va_arg(args,ulinux_si);
			break;
		case FMT_TYPE_PRECISION:/*get precision from the argument list*/
			spec.precision=(ulinux_s16)va_arg(args,ulinux_si);
			break;
		case FMT_TYPE_CHAR:{
			ulinux_u8 c;
			if(!(spec.flags&LEFT)){
				ulinux_loop{
					if(--spec.field_width<=0) break;
					if(str<end) *str=' ';
					++str;
				}
			}
			c=(ulinux_u8)va_arg(args,ulinux_si);
			if(str<end) *str=c;
			++str;
			ulinux_loop{
				if(--spec.field_width<=0) break;
				if(str<end) *str=' ';
				++str;
			}
			break;
		}
		case FMT_TYPE_STR:
			str=string(str,end,va_arg(args,ulinux_u8*),spec);
			break;
		case FMT_TYPE_PTR:
			str=pointer(fmt+1,str,end,va_arg(args,void*),spec);
			/*alnums are extension to pointer fmt (see cmts)*/
			ulinux_loop{
				if(!ulinux_is_alnum(*fmt)) break;
				fmt++;
			}
			break;
		case FMT_TYPE_PERCENT_CHAR:
			if(str<end) *str='%';
			++str;
			break;
		case FMT_TYPE_INVALID:
			if(str<end) *str='%';
			++str;
			break;
		case FMT_TYPE_NRCHARS:{
			ulinux_u8 qualifier;
			qualifier=spec.qualifier;
      
			if(qualifier=='L'){ 
				ulinux_u64 *ip;
				ip=va_arg(args,ulinux_u64*);
				*ip=(str-buf);
			}else if(qualifier=='l'){
				ulinux_u64 *ip;
				ip=va_arg(args,ulinux_u64*);
				*ip=(str-buf);
			}else if(TOLOWER(qualifier)=='z'){
				ulinux_u64 *ip;
				ip=va_arg(args,ulinux_u64*);
				*ip=(str-buf);
			}else{
				ulinux_u64 *ip;
				ip=va_arg(args,ulinux_u64*);
				*ip=(str-buf);
			}
			break;
		}
		default:
			switch(spec.type){
			case FMT_TYPE_LONG_LONG:
				num=va_arg(args,ulinux_s64);
				break;
			case FMT_TYPE_ULONG:
				if(BITS_PER_LONG==64)
					num=va_arg(args,ulinux_u64);
				else
					num=va_arg(args,ulinux_u32);
				break;
			case FMT_TYPE_LONG:
				if(BITS_PER_LONG==64)
					num=va_arg(args,ulinux_s64);
				else
					num=va_arg(args,ulinux_s32);
				break;
			case FMT_TYPE_SIZE_T:
				num=va_arg(args,ulinux_u64);
				break;
			case FMT_TYPE_PTRDIFF:
				num=va_arg(args,ulinux_u64);
				break;
			case FMT_TYPE_UBYTE:
				num=(ulinux_u8)va_arg(args,ulinux_u64);
				break;
			case FMT_TYPE_BYTE:
				num=(ulinux_s8)va_arg(args,ulinux_s64);
				break;
			case FMT_TYPE_USHORT:
				num=(ulinux_u16)va_arg(args,ulinux_u64);
				break;
			case FMT_TYPE_SHORT:
				num=(ulinux_s16)va_arg(args,ulinux_s64);
				break;
			case FMT_TYPE_INT:
				num=va_arg(args,ulinux_s32);
				break;
			default:
				num=va_arg(args,ulinux_u32);
			}
			str=number(str,end,num,spec);
		}
	}

	if(sz>0){/*set the terminating null byte, if not truncated*/
		/*if(str<end) *str=0;else end[-1]=0;*/
		if(str<end) *str=0;
	}
	/*the trailing null byte doesn't count towards the total*/
	return str-buf;
}

/*
snprintf - Format a string and place it in a buffer
@buf: The buffer to place the result into
@size: The size of the buffer, including the trailing null space
@fmt: The format string to use
@...: Arguments for the format string

The return value is the number of characters which would be
generated for the given input, excluding the trailing null,
as per ISO C99.  If the return is greater than or equal to
@size, the resulting string is truncated.

See the vsnprintf() documentation for format string extensions over C99.
*/
ulinux_u64 ulinux_snprintf(ulinux_u8 *buf,ulinux_u64 sz,ulinux_u8 *fmt,...)
{
	ulinux_u64 i;
	va_list args;
	
	va_start(args,fmt);
	i=ulinux_vsnprintf(buf,sz,fmt,args);
	va_end(args);
	return i;
}

/*dprintf, print formatted output to file*/
ulinux_s8 ulinux_dprintf(ulinux_si f,ulinux_u8 *buf,ulinux_u64 sz,
							ulinux_u8 *fmt,...)
{
	ulinux_u64 len;
	va_list args;
	ulinux_sl r;
	
	va_start(args,fmt);
	len=ulinux_vsnprintf(buf,sz,fmt,args);
	va_end(args);
	
	ulinux_loop{
		r=ulinux_sysc(write,3,f,buf,len);
		if(r!=-ULINUX_EINTR&&r!=-ULINUX_EAGAIN) break;
	}
	if(ULINUX_ISERR(r)||(r!=(ulinux_sl)len)) return -2;
	return 0;
}
