#ifndef ULINUX_UTILS_ASCII_ASCII_H
#define ULINUX_UTILS_ASCII_ASCII_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ulinux_dec2ut_blk  ulinux_dec2u8
#define ulinux_dec2us_blk  ulinux_dec2u16_blk
#define ulinux_dec2us      ulinux_dec2u16
#if BITS_PER_LONG==64
#define ulinux_dec2ul_blk  ulinux_dec2u64_blk
#define ulinux_dec2ul      ulinux_dec2u64
#else
#define ulinux_dec2ul_blk  ulinux_dec2u32_blk
#define ulinux_dec2ul      ulinux_dec2u32
#endif
#define ulinux_dec2ull_blk ulinux_dec2u64_blk
#define ulinux_dec2ull     ulinux_dec2u64

/* XXX:may use linux ctype ascii table */
static inline ulinux_u8 ulinux_is_digit(ulinux_u8 c)
{
  if('0'<=c&&c<='9') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_alnum(ulinux_u8 c)
{
  if('0'<=c&&c<='9') return 1;
  if('A'<=c&&c<='Z') return 1;
  if('a'<=c&&c<='z') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_lower(ulinux_u8 c)
{
  if('a'<=c&&c<='z') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_upper(ulinux_u8 c)
{
  if('A'<=c&&c<='Z') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_alpha(ulinux_u8 c)
{
  if('A'<=c&&c<='Z') return 1;
  if('a'<=c&&c<='z') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_blank(ulinux_u8 c)
{
  if(c==' '||c=='\t') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_space(ulinux_u8 c)
{
  if(('\t'<=c&&c<='\r')||c==' ') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_xdigit(ulinux_u8 c)
{
  if('0'<=c&&c<='9') return 1;
  if('A'<=c&&c<='F') return 1;
  if('a'<=c&&c<='f') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_cntrl(ulinux_u8 c)
{
  if(c<=0x1f||c==0x7f) return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_punct(ulinux_u8 c)
{
  if(('!'<=c&&c<='/')||(':'<=c&&c<='@')||('['<=c&&c<='`')||('{'<=c&&c<='~'))
    return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_hex(ulinux_u8 c)
{
  if('0'<=c&&c<='9') return '0';
  if('A'<=c&&c<='F') return 'A';
  if('a'<=c&&c<='z') return 'a';
  return 0;
}

static inline ulinux_u8 ulinux_is_print(ulinux_u8 c)
{
  if(' '<=c&&c<='~') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_is_graph(ulinux_u8 c)
{
  if('!'<=c&&c<='~') return 1;
  return 0;
}

static inline ulinux_u8 ulinux_2upper(ulinux_u8 c)
{
  if(ulinux_is_lower(c)) c&=~0x20;
  return c;
}

static inline ulinux_u8 ulinux_2lower(ulinux_u8 c)
{
  if(ulinux_is_upper(c)) c|=0x20;
  return c;
}
#endif
