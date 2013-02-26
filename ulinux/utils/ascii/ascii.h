#ifndef ULINUX_UTILS_ASCII_ASCII_H
#define ULINUX_UTILS_ASCII_ASCII_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define u_a_strict_dec2ut_blk  u_a_strict_dec2u8
#define u_a_strict_dec2us_blk  u_a_strict_dec2u16_blk
#define u_a_strict_dec2us      u_a_strict_dec2u16
#if BITS_PER_LONG==64
#define u_a_strict_dec2ul_blk  u_a_strict_dec2u64_blk
#define u_a_strict_dec2ul      u_a_strict_dec2u64
#else
#define u_a_strict_dec2ul_blk  u_a_strict_dec2u32_blk
#define u_a_strict_dec2ul      u_a_strict_dec2u32
#endif
#define u_a_strict_dec2ull_blk u_a_strict_dec2u64_blk
#define u_a_strict_dec2ull     u_a_strict_dec2u64

//XXX:may use linux ctype ascii table
static inline k_ut u_a_is_digit(k_u8 c)
{
  if('0'<=c&&c<='9') return 1;
  return 0;
}

static inline k_u8 u_a_is_hex(k_u8 c)
{
  if('0'<=c&&c<='9') return '0';
  if('A'<=c&&c<='F') return 'A';
  if('a'<=c&&c<='z') return 'a';
  return 0;
}

static inline k_ut u_a_is_alnum(k_u8 c)
{
  if('0'<=c&&c<='9') return 1;
  if('A'<=c&&c<='Z') return 1;
  if('a'<=c&&c<='z') return 1;
  return 0;
}

static inline k_ut u_a_is_lower(k_u8 c)
{
  if('a'<=c&&c<='z') return 1;
  return 0;
}

static inline k_u8 u_a_2upper(k_u8 c)
{
  if(u_a_is_lower(c)) c-='a'-'A';
  return c;
}
#endif
