//**********************************************************************************************
//*this code is protected by the GNU affero GPLv3
//*ripper:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*ripped and sort of ported to user space from Linux kernel
//**********************************************************************************************

//linux/lib/vsprintf.c
//
// Copyright (C) 1991, 1992  Linus Torvalds
//
//
//vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
//
//Wirzenius wrote this portably, Torvalds fucked it up :-)
//
//
//Fri Jul 13 2001 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
//- changed to provide snprintf and vsnprintf functions
//So Feb  1 16:51:32 CET 2004 Juergen Quade <quade@hsnr.de>
//- scnprintf and vscnprintf

#include <stdarg.h>

#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/mmap.h>
#include <ulinux/utils/div.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/endian.h>
#include <ulinux/ipv6.h>
#include <ulinux/utils/ipv6.h>
#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>

//works only for digits and letters, but small and fast
#define TOLOWER(x) ((x)|0x20)

#define ZEROPAD	1 //pad with zero
#define SIGN	2 //unsigned/signed long
#define PLUS	4 //show plus
#define SPACE	8 //space if plus
#define LEFT	16//left justified
#define SMALL	32//use lowercase in hex (must be 32 == 0x20)
#define SPECIAL	64//prefix hex with "0x", octal with "0"

enum format_type{
  FORMAT_TYPE_NONE,//Just a string part
  FORMAT_TYPE_WIDTH,
  FORMAT_TYPE_PRECISION,
  FORMAT_TYPE_CHAR,
  FORMAT_TYPE_STR,
  FORMAT_TYPE_PTR,
  FORMAT_TYPE_PERCENT_CHAR,
  FORMAT_TYPE_INVALID,
  FORMAT_TYPE_LONG_LONG,
  FORMAT_TYPE_ULONG,
  FORMAT_TYPE_LONG,
  FORMAT_TYPE_UBYTE,
  FORMAT_TYPE_BYTE,
  FORMAT_TYPE_USHORT,
  FORMAT_TYPE_SHORT,
  FORMAT_TYPE_UINT,
  FORMAT_TYPE_INT,
  FORMAT_TYPE_NRCHARS,
  FORMAT_TYPE_SIZE_T,
  FORMAT_TYPE_PTRDIFF
};

struct printf_spec {
  k_u8  type;       //format_type enum
  k_u8  flags;      //flags to number()
  k_u8  base;       //number base, 8, 10 or 16 only
  k_u8  qualifier;  //number qualifier, one of 'hHlLtzZ'
  k_s16 field_width;//width of output field, -1 means not present or ignored for some reason
  k_s16 precision;  //# of digits/chars, -1 means not present or ignored for some reason
};

//same with if's removed. Always emits five digits
static k_u8 *_put_dec_full(k_u8 *buf,k_u q)
{
  //BTW, if q is in [0,9999], 8-bit ints will be enough,
  //but anyway, gcc produces better code with full-sized ints
  k_u d3,d2,d1,d0;
  d1=(q>>4)&0xf;
  d2=(q>>8)&0xf;
  d3=(q>>12);

  //Possible ways to approx. divide by 10
  //gcc -O2 replaces multiply with shifts and adds
  //(x * 0xcd) >> 11: 11001101 - shorter code than * 0x67 (on i386)
  //(x * 0x67) >> 10:  1100111
  //(x * 0x34) >> 9:    110100 - same
  //(x * 0x1a) >> 8:     11010 - same
  //(x * 0x0d) >> 7:      1101 - same, shortest code (on i386)
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
  q=(d3*0xcd)>>11;//- shorter code
  //q = (d3 * 0x67) >> 10; - would also work
  d3=d3-10*q;
  *buf++=d3+'0';
  *buf++=q+'0';
  return buf;
}

//Decimal conversion is by far the most typical, and is used
//for /proc and /sys data. This directly impacts e.g. top performance
//with many processes running. We optimize it for speed
//using code from
//http://www.cs.uiowa.edu/~jones/bcd/decimal.html
//(with permission from the author, Douglas W. Jones).

//Formats correctly any integer in [0,99999].
//Outputs from one to five digits depending on input.
//On i386 gcc 4.1.2 -O2: ~250 bytes of code.
static k_u8 *_put_dec_trunc(k_u8 *buf,k_u q)
{
  k_u d3,d2,d1,d0;
  d1=(q>>4)&0xf;
  d2=(q>>8)&0xf;
  d3=(q>>12);
  
  d0=6*(d3+d2+d1)+(q&0xf);
  q=(d0*0xcd)>>11;
  d0=d0-10*q;
  *buf++=d0+'0';//least significant digit
  d1=q+9*d3+5*d2+d1;
  if(d1!=0){
    q=(d1*0xcd)>>11;
    d1=d1-10*q;
    *buf++=d1+'0';//next digit
    
    d2=q+2*d2;
    if((d2!=0)||(d3!= 0)){
      q=(d2*0xd)>>7;
      d2=d2-10*q;
      *buf++=d2+'0';//next digit
      
      d3=q+4*d3;
      if(d3!=0){
        q=(d3*0xcd)>>11;
        d3=d3-10*q;
        *buf++=d3+'0';//next digit
        if(q!=0) *buf++=q+'0';//most sign. digit
      }
    }
  }
  return buf;
}

//no inlining helps gcc to use registers better
static k_u8 *_put_dec(k_u8 *buf,k_ull num)
{
  for(;;){
    k_u rem;
    if(num<100000) return _put_dec_trunc(buf,num);
    rem=u_do_div(num,100000);
    buf=_put_dec_full(buf,rem);
  }
}

static k_u8 *_number(k_u8 *buf,k_u8 *end,k_ull num,struct printf_spec spec)
{
  //we are called with base 8, 10 or 16, only, thus don't need "G..."
  //locase = 0 or 0x20. ORing digits or letters with 'locase'
  //produces same digits or (maybe lowercased) letters
  k_u8 locase=(spec.flags&SMALL);
  if(spec.flags&LEFT) spec.flags&=~ZEROPAD;
  k_u8 sign=0;
  if(spec.flags&SIGN) {
    if((k_ll)num<0){
      sign='-';
      num=-(k_ll)num;
      spec.field_width--;
    }else if(spec.flags&PLUS){
      sign='+';
      spec.field_width--;
    }else if(spec.flags&SPACE){
      sign=' ';
      spec.field_width--;
    }
  }
  k_ut need_pfx=((spec.flags&SPECIAL)&&spec.base!=10);
  if(need_pfx){
    spec.field_width--;
    if(spec.base==16) spec.field_width--;
  }

  //generate full string in tmp[], in reverse order
  static k_u8 digits[16]="0123456789ABCDEF";//"GHIJKLMNOPQRSTUVWXYZ";
  k_u8 tmp[66];//octal max digit number for unsigned 64 bits plus sign
  k_i i=0;
  if(num==0) tmp[i++]='0';
  //Generic code, for any base:
  //else do {
  //tmp[i++] = (digits[do_div(num,base)] | locase);
  //} while (num != 0);
  else if(spec.base!=10){//8 or 16
    k_i mask=spec.base-1;
    k_i shift=3;
    
    if(spec.base==16) shift=4;
    do{
      tmp[i++]=(digits[((k_u8)num)&mask]|locase);
      num>>=shift;
    }while(num);
  }else i=_put_dec(tmp,num)-tmp;//base 10

  //printing 100 using %2d gives "100", not "00"
  if(i>spec.precision) spec.precision=i;
  //leading space padding
  spec.field_width-=spec.precision;
  if(!(spec.flags&(ZEROPAD+LEFT))){
    while(--spec.field_width>=0){
      if(buf<end) *buf=' ';
      ++buf;
    }
  }
  //sign
  if(sign){
    if(buf<end) *buf=sign;
    ++buf;
  }
  //"0x" / "0" prefix
  if(need_pfx){
    if(buf<end) *buf='0';
    ++buf;
    if(spec.base==16){
      if(buf<end) *buf=('X'|locase);
      ++buf;
    }
  }
  //zero or space padding
  if(!(spec.flags & LEFT)){
    k_u8 c=(spec.flags&ZEROPAD)?'0':' ';
    while(--spec.field_width>=0){
      if(buf<end) *buf=c;
      ++buf;
    }
  }
  //hmm even more zero padding?
  while(i<=--spec.precision){
    if(buf<end) *buf='0';
    ++buf;
  }
  //actual digits of result
  while(--i>=0){
    if(buf<end) *buf=tmp[i];
    ++buf;
  }
  //trailing space padding
  while(--spec.field_width>=0){
    if(buf<end) *buf=' ';
    ++buf;
  }
  return buf;
}

static k_u8 *_string(k_u8 *buf,k_u8 *end,k_u8 *s,struct printf_spec spec)
{
  if((k_ul)s<K_PAGE_SZ) s=(k_u8*)"(null)";//XXX:why excluding first page *here*??

  k_ul len;
  if(!u_a_strnlen(&len,s,spec.precision)) len=spec.precision;

  if(!(spec.flags&LEFT))//it's RIGHT
    while((k_s16)len<spec.field_width--){
      if(buf<end) *buf=' ';
      ++buf;
    }

  for(k_ul i=0;i<len;++i){
    if(buf<end) *buf=*s;
    ++buf;++s;
  }

  while((k_s16)len<spec.field_width--){
    if(buf<end) *buf=' ';
    ++buf;
  }
  return buf;
}

static k_u8 _hex_asc[]="0123456789abcdef";
#define _hex_asc_lo(x) _hex_asc[((x)&0x0f)]
#define _hex_asc_hi(x) _hex_asc[((x)&0xf0)>>4]

static inline k_u8 *_pack_hex_byte(k_u8 *buf,k_u8 byte)
{
  *buf++=_hex_asc_hi(byte);
  *buf++=_hex_asc_lo(byte);
  return buf;
}

static k_u8 *_mac_address_string(k_u8 *buf,k_u8 *end,k_u8 *addr,struct printf_spec spec,
                                 k_u8 *fmt)
{
  k_u8 mac_addr[sizeof("xx:xx:xx:xx:xx:xx")];
  k_u8 *p=mac_addr;
  k_u8 separator;
  
  if(fmt[1]=='F') separator='-';//FDDI canonical format
  else separator=':';
  
  for(int i=0;i< 6;++i){
    p=_pack_hex_byte(p,addr[i]);
    if(fmt[0]=='M'&&i!=5) *p++=separator;
  }
  *p=0;
  return _string(buf,end,mac_addr,spec);
}

static k_u8 *_ip4_string(k_u8 *p,k_u8 *addr,k_u8 *fmt)
{
  k_i index,step;
  switch(fmt[2]){
    case 'h':
#     ifdef __BIG_ENDIAN
        index=0;
        step=1;
#     else
        index=3;
        step=-1;
#     endif
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

  k_ut leading_zeros=(fmt[0]=='i');
  for(k_i i=0;i<4;++i){
    k_u8 temp[3];//hold each IP quad in reverse order
    k_ut digits=_put_dec_trunc(temp,addr[index])-temp;
    if(leading_zeros){
      if(digits<3) *p++='0';
      if(digits<2) *p++='0';
    }
    //reverse the digits in the quad
    while(digits--) *p++=temp[digits];
    if(i<3) *p++='.';
    index+=step;
  }
  *p=0;
  return p;
}

static k_u8 *_ip6_compressed_string(k_u8 *p,k_u8 *addr)
{
  k_ut needcolon=0;
  struct k_in6_addr in6;
  
  u_memcpy(&in6,addr,sizeof(in6));
  
  k_ut useIPv4=u_ipv6_addr_v4mapped(&in6)||u_ipv6_addr_is_isatap(&in6);
  
  k_ut zerolength[8];
  u_memset(zerolength,0,sizeof(zerolength));
 
  k_ut range; 
  if(useIPv4) range=6;
  else range=8;
  
  //find position of longest 0 run
  for(k_ut i=0;i<range;++i)
    for(k_ut j=i;j<range;++j){
      if(in6.s6_addr16[j]!=0) break;
      zerolength[i]++;
    }

  k_ut longest=1;
  k_ut colonpos=-1;
  for(k_ut i=0;i<range;++i){
    if(zerolength[i]>longest){
      longest=zerolength[i];
      colonpos=i;
    }
  }
  
  //emit address
  for(k_ut i=0;i<range;++i){
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
    //hex u16 without leading 0s
    k_u16 word=u_be162cpu(in6.s6_addr16[i]);
    k_u8 hi=word>>8;
    k_u8 lo=word&0xff;
    if(hi){
      if(hi>0x0f) p=_pack_hex_byte(p,hi);
      else *p++=_hex_asc_lo(hi);
      p=_pack_hex_byte(p,lo);
    }else if(lo>0x0f)
      p=_pack_hex_byte(p,lo);
    else
      *p++=_hex_asc_lo(lo);
    needcolon=1;
  }
  
  if(useIPv4){
    if(needcolon) *p++=':';
    p=_ip4_string(p,&in6.s6_addr[12],(k_u8*)"I4");
  }
  *p=0;
  return p;
}

static k_u8 *_ip6_string(k_u8 *p,k_u8 *addr,k_u8 *fmt)
{
  for(k_i i=0;i<8;++i){
    p=_pack_hex_byte(p,*addr++);
    p=_pack_hex_byte(p,*addr++);
    if(fmt[0]=='I'&&i!=7) *p++=':';
  }
  *p=0;
  return p;
}

static k_u8 *_ip6_addr_string(k_u8 *buf,k_u8 *end,k_u8 *addr,struct printf_spec spec,k_u8 *fmt)
{
  k_u8 ip6_addr[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255")];

  if(fmt[0]=='I'&&fmt[2]=='c') _ip6_compressed_string(ip6_addr,addr);
  else _ip6_string(ip6_addr,addr,fmt);
  return _string(buf,end,ip6_addr,spec);
}

static k_u8 *_ip4_addr_string(k_u8 *buf,k_u8 *end,k_u8 *addr,struct printf_spec spec,k_u8 *fmt)
{
  k_u8 ip4_addr[sizeof("255.255.255.255")];
  _ip4_string(ip4_addr,addr,fmt);
  return _string(buf,end,ip4_addr,spec);
}

static k_u8 *_uuid_string(k_u8 *buf,k_u8 *end,k_u8 *addr,struct printf_spec spec,k_u8 *fmt)
{
  static k_u8 be[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  static k_u8 le[16]={3,2,1,0,5,4,7,6,8,9,10,11,12,13,14,15};
  k_u8 *index=be;
  k_ut uc=0;
  
  switch(*(++fmt)){
    case 'L':
      uc=1;//fall-through
    case 'l':
      index=le;
      break;
    case 'B':
      uc=1;
      break;
  }
  
  k_u8 uuid[sizeof("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx")];
  k_u8 *p=uuid;
  for(k_i i=0;i<16;++i){
    p=_pack_hex_byte(p,addr[index[i]]);
    switch(i){
      case 3:
      case 5:
      case 7:
      case 9:
        *p++='-';
        break;
    }
  }
  *p=0;
  
  if(uc){
    p=uuid;
    do{
      *p=u_a_2upper(*p);
    }while(*(++p));
  }
  return _string(buf,end,uuid,spec);
}

//Show a '%p' thing.  A kernel extension is that the '%p' is followed
//by an extra set of alphanumeric characters that are extended format
//specifiers.
//
//Right now we handle:
//
//- 'M' For a 6-byte MAC address, it prints the address in the
//      usual colon-separated hex notation
//- 'm' For a 6-byte MAC address, it prints the hex address without colons
//- 'MF' For a 6-byte MAC FDDI address, it prints the address
//      with a dash-separated hex notation
//- 'I' [46] for IPv4/IPv6 addresses printed in the usual way
//      IPv4 uses dot-separated decimal without leading 0's (1.2.3.4)
//      IPv6 uses colon separated network-order 16 bit hex with leading 0's
//- 'i' [46] for 'raw' IPv4/IPv6 addresses
//      IPv6 omits the colons (01020304...0f)
//      IPv4 uses dot-separated decimal with leading 0's (010.123.045.006)
//- '[Ii]4[hnbl]' IPv4 addresses in host, network, big or little endian order
//- 'I6c' for IPv6 addresses printed as specified by
//      http://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-00
//- 'U' For a 16 byte UUID/GUID, it prints the UUID/GUID in the form
//      "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
//      Options for %pU are:
//        b big endian lower case hex (default)
//        B big endian UPPER case hex
//        l little endian lower case hex
//        L little endian UPPER case hex
//          big endian output byte order is:
//            [0][1][2][3]-[4][5]-[6][7]-[8][9]-[10][11][12][13][14][15]
//          little endian output byte order is:
//            [3][2][1][0]-[5][4]-[7][6]-[8][9]-[10][11][12][13][14][15]
//
//Note: The difference between 'S' and 'F' is that on ia64 and ppc64
//function pointers are really function descriptors, which contain a
//pointer to the real address.
static k_u8 *_pointer(k_u8 *fmt,k_u8 *buf,k_u8 *end,void *ptr,struct printf_spec spec)
{
  if(!ptr) return _string(buf,end,(k_u8*)"(null)",spec);

  switch(*fmt){
    case 'M'://colon separated: 00:01:02:03:04:05
    case 'm'://contiguous: 000102030405
      return _mac_address_string(buf,end,ptr,spec,fmt);//[mM]F (FDDI, bit reversed)
    case 'I'://Formatted IP supported
             //4:  1.2.3.4
             //6:  0001:0203:...:0708
             //6c: 1::708 or 1::1.2.3.4
    case 'i'://contiguous:
             //4: 001.002.003.004
             //6: 000102...0f
      switch(fmt[1]){
        case '6':
          return _ip6_addr_string(buf,end,ptr,spec,fmt);
        case '4':
          return _ip4_addr_string(buf,end,ptr,spec,fmt);
      }
      break;
    case 'U':
      return _uuid_string(buf,end,ptr,spec,fmt);
  }

  spec.flags|=SMALL;
  if(spec.field_width==-1){
    spec.field_width=2*sizeof(void*);
    spec.flags|=ZEROPAD;
  }
  spec.base=16;
  return _number(buf,end,(k_ull)ptr,spec);
}

static inline k_u16 _a2u16(k_u8 **s)//trusted
{
  k_u16 i=0;
  while(u_a_is_digit(**s)) i=i*10+*((*s)++)-'0';
  return i;
}

//Helper function to decode printf style format.
//Each call decode a token from the format and return the
//number of characters read (or likely the delta where it wants
//to go on the next call).
//The decoded token is returned through the parameters
//
//'h', 'l', or 'L' for integer fields
//'z' support added 23/7/1999 S.H.
//'z' changed to 'Z' --davidm 1/25/99
//'t' added for ptrdiff_t
//
//@fmt: the format string
//@type of the token returned
//@flags: various flags such as +, -, # tokens..
//@field_width: overwritten width
//@base: base of the number (octal, hex, ...)
//@precision: precision of a number
//@qualifier: qualifier of a number (long, size_t, ...)
static k_i _format_decode(k_u8 *fmt,struct printf_spec *spec)
{
  k_u8 *start=fmt;

  //--------------------------------------------------------------------------------------------
  if(spec->type==FORMAT_TYPE_WIDTH){//we finished early by reading the field width
    if(spec->field_width<0){
      spec->field_width=-spec->field_width;
      spec->flags|=LEFT;
    }
    spec->type=FORMAT_TYPE_NONE;
    goto precision;
  }

  if(spec->type==FORMAT_TYPE_PRECISION){//we finished early by reading the precision
    if(spec->precision<0) spec->precision=0;
    spec->type=FORMAT_TYPE_NONE;
    goto qualifier;
  }
  //--------------------------------------------------------------------------------------------

  spec->type=FORMAT_TYPE_NONE;//by default
  for(;*fmt;++fmt) if(*fmt=='%')break;
  
  if(fmt!=start||!*fmt) return fmt-start;//return the current non-format string

  spec->flags=0;//process flags

  for(;;){//this also skips first '%'
    k_ut found=1;
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

  //get field width
  spec->field_width=-1;

  if(u_a_is_digit(*fmt)) spec->field_width=(k_s16)_a2u16(&fmt);
  else if(*fmt=='*'){
    spec->type=FORMAT_TYPE_WIDTH;//get it from the argument list
    return ++fmt-start;
  }

  //get the precision
precision:
  spec->precision=-1;
  if(*fmt=='.'){
    ++fmt;
    if(u_a_is_digit(*fmt)){
      spec->precision=(k_s16)_a2u16(&fmt);
      if(spec->precision<0) spec->precision=0;
    }else if(*fmt=='*'){//it's the next argument
      spec->type=FORMAT_TYPE_PRECISION;
      return ++fmt-start;
    }else spec->precision=0;
  }

qualifier:
  //get the conversion qualifier
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

  spec->base=10;//default base:2/8/10/16 wanted?
  switch(*fmt){
    case 'c':
      spec->type=FORMAT_TYPE_CHAR;
      return ++fmt-start;
    case 's':
      spec->type=FORMAT_TYPE_STR;
      return ++fmt-start;
    case 'p':
      spec->type=FORMAT_TYPE_PTR;
      return fmt-start;
      //skip alnum
    case 'n':
      spec->type=FORMAT_TYPE_NRCHARS;
      return ++fmt-start;
    case '%':
      spec->type=FORMAT_TYPE_PERCENT_CHAR;
      return ++fmt-start;
    //integer number formats - set up the flags and "break"
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
      spec->type=FORMAT_TYPE_INVALID;
      return fmt-start;
  }

  if(spec->qualifier=='L') spec->type=FORMAT_TYPE_LONG_LONG;
  else if(spec->qualifier=='l'){
    if(spec->flags&SIGN) spec->type=FORMAT_TYPE_LONG;
    else spec->type=FORMAT_TYPE_ULONG;
  }else if(TOLOWER(spec->qualifier)=='z'){
    spec->type=FORMAT_TYPE_SIZE_T;
  }else if(spec->qualifier=='t'){
    spec->type=FORMAT_TYPE_PTRDIFF;
  }else if(spec->qualifier=='H'){
    if(spec->flags&SIGN) spec->type=FORMAT_TYPE_BYTE;
    else spec->type=FORMAT_TYPE_UBYTE;
  }else if(spec->qualifier=='h'){
    if(spec->flags&SIGN) spec->type=FORMAT_TYPE_SHORT;
    else spec->type=FORMAT_TYPE_USHORT;
  }else{
    if(spec->flags&SIGN) spec->type=FORMAT_TYPE_INT;
    else spec->type=FORMAT_TYPE_UINT;
  }
  return ++fmt-start;
}

//vsnprintf - Format a string and place it in a buffer
//@buf: The buffer to place the result into
//@sz: The size of the buffer, including the trailing null space
//@fmt: The format string to use
//@args: Arguments for the format string
//
//This function follows C99 vsnprintf, but has some extensions:
//%pM output a 6-byte MAC address with colons
//%pm output a 6-byte MAC address without colons
//%pI4 print an IPv4 address without leading zeros
//%pi4 print an IPv4 address with leading zeros
//%pI6 print an IPv6 address with colons
//%pi6 print an IPv6 address without colons
//%pI6c print an IPv6 address as specified by
//  http://tools.ietf.org/html/draft-ietf-6man-text-addr-representation-00
//%pU[bBlL] print a UUID/GUID in big or little endian using lower or upper
//  case.
//%n is ignored
//
//The return value is the number of characters which would
//be generated for the given input, excluding the trailing
//'\0', as per ISO C99. If you want to have the exact
//number of characters written into @buf as return value
//(not including the trailing '\0'), use vscnprintf(). If the
//return is greater than or equal to @size, the resulting
//string is truncated.
//
//Call this function if you are already dealing with a va_list.
//You probably want snprintf() instead.
//SECURITY:fmt is **trusted**,argument related to %fmt are **trusted**
k_l u_a_vsnprintf(k_u8 *buf,k_ul sz,k_u8 *fmt,va_list args)
{
  k_ull num;
  struct printf_spec spec={0,0,0,0,0,0};

  k_u8 *str=buf;
  k_u8 *end=buf+sz;//point to the byte *after* the buffer last byte

  while(*fmt){
    k_u8 *old_fmt=fmt;
    k_ul read=_format_decode(fmt,&spec);

    fmt+=read;

    switch(spec.type){
      case FORMAT_TYPE_NONE:{
        k_ul copy=read;
        if(str<end){
          if((k_ptrdiff)copy>end-str) copy=end-str;
          u_memcpy(str,old_fmt,copy);
        }
        str+=read;
        break;
      }
      case FORMAT_TYPE_WIDTH://get field width from the argument list
        spec.field_width=(k_s16)va_arg(args,int);
        break;
      case FORMAT_TYPE_PRECISION://get precision from the argument list
        spec.precision=(k_s16)va_arg(args,int);
        break;
      case FORMAT_TYPE_CHAR:{
        k_u8 c;
        if(!(spec.flags&LEFT)){
          while(--spec.field_width>0){
            if(str<end) *str=' ';
            ++str;
          }
        }
        c=(k_u8)va_arg(args,int);
        if(str<end) *str=c;
        ++str;
        while(--spec.field_width>0){
          if(str<end) *str=' ';
          ++str;
        }
        break;
      }
      case FORMAT_TYPE_STR:
        str=_string(str,end,va_arg(args,k_u8*),spec);
        break;
      case FORMAT_TYPE_PTR:
        str=_pointer(fmt+1,str,end,va_arg(args,void*),spec);
        while(u_a_is_alnum(*fmt)) fmt++;//alnums are extension to pointer fmt (see cmts)
        break;
      case FORMAT_TYPE_PERCENT_CHAR:
        if(str<end) *str='%';
        ++str;
        break;
      case FORMAT_TYPE_INVALID:
        if(str<end) *str='%';
        ++str;
        break;
      case FORMAT_TYPE_NRCHARS:{
        k_u8 qualifier=spec.qualifier;
      
        if(qualifier=='L'){ 
          k_ull *ip=va_arg(args,k_ull*);
          *ip=(str-buf);
        }else if(qualifier=='l'){
          k_ul *ip=va_arg(args,k_ul*);
          *ip=(str-buf);
        }else if(TOLOWER(qualifier)=='z'){
          k_sz *ip=va_arg(args,k_sz*);
          *ip=(str-buf);
        }else{
          k_u *ip=va_arg(args,k_u*);
          *ip=(str-buf);
        }
        break;
      }

      default:
        switch(spec.type){
          case FORMAT_TYPE_LONG_LONG:
            num=va_arg(args,k_ll);
            break;
          case FORMAT_TYPE_ULONG:
            num=va_arg(args,k_ul);
            break;
          case FORMAT_TYPE_LONG:
            num=va_arg(args,k_l);
            break;
          case FORMAT_TYPE_SIZE_T:
            num=va_arg(args,k_sz);
            break;
          case FORMAT_TYPE_PTRDIFF:
            num=va_arg(args,k_ptrdiff);
            break;
          case FORMAT_TYPE_UBYTE:
            num=(k_u8)va_arg(args,int);
            break;
          case FORMAT_TYPE_BYTE:
            num=(k_s8)va_arg(args,int);
            break;
          case FORMAT_TYPE_USHORT:
            num=(k_us)va_arg(args,int);
            break;
          case FORMAT_TYPE_SHORT:
            num=(k_s)va_arg(args,int);
            break;
          case FORMAT_TYPE_INT:
            num=va_arg(args,k_i);
            break;
          default:
            num=va_arg(args,k_u);
        }
        str=_number(str,end,num,spec);
    }
  }

  if(sz>0){//set the terminating null byte
    if(str<end) *str=0;else end[-1]=0;
  }
  return str-buf;//the trailing null byte doesn't count towards the total
}

//snprintf - Format a string and place it in a buffer
//@buf: The buffer to place the result into
//@size: The size of the buffer, including the trailing null space
//@fmt: The format string to use
//@...: Arguments for the format string
//
//The return value is the number of characters which would be
//generated for the given input, excluding the trailing null,
//as per ISO C99.  If the return is greater than or equal to
//@size, the resulting string is truncated.
//
//See the vsnprintf() documentation for format string extensions over C99.
k_l u_a_snprintf(k_u8 *buf,k_ul sz,k_u8 *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  k_l l=u_a_vsnprintf(buf,sz,fmt,args);
  va_end(args);
  return l;
}

//dprintf, print formatted output to file.
k_i u_a_dprintf(k_i f,k_u8 *buf,k_ul sz,k_u8 *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  k_l len=u_a_vsnprintf(buf,sz,fmt,args);
  va_end(args);

  //k_l r=sysc(write,3,f,buf,len);
  //if(K_ISERR(r)) return -2;
  k_l r;
  do{
    r=sysc(write,3,f,buf,len);
  }while(r==-K_EINTR||r==-K_EAGAIN);
  if(K_ISERR(r)||(r!=len)) return -2;
  return 0;
}
