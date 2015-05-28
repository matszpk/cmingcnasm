#ifndef ULINUX_UTILS_MATHS_H
#define ULINUX_UTILS_MATHS_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
/*
will round to nearest integer
.5 policy is round away from zero
*/
static inline ulinux_f32 ulinux_f32_round_near(ulinux_f32 f)
{
  ulinux_u32 *u=(ulinux_u32*)&f;
  ulinux_u8 e_biased=*u>>23&0xff;/*biased exponent*/

  if(e_biased==0xff) return f;/*+/-inf or nan, returned as is*/
  if(e_biased==0x00) return 0.0f;/*subnormal floats are in ]-0.5;0.5[*/

  ulinux_s8 e=(ulinux_s8)((ulinux_s16)e_biased-127);

  /*no frac then integer*/
  if(e>=23) return f;
  /*f in ]-0.5;0.5[*/
  if(e<-1) return 0.0f;
  /*hidden bit always 1, f in ]-1;-0.5] U [0.5;1[, then round away from 0*/
  if(e==-1) return *u&0x80000000?-1.0f:1.0f;

  ulinux_s8 first_frac_bit_idx=22-e;
  ulinux_u8 round_away_from_zero=(*u>>first_frac_bit_idx)&1;
  *u&=~((1<<(first_frac_bit_idx+1))-1);/*zero frac from significand*/
  if(round_away_from_zero) f+=(*u&0x80000000?-1.0f:1.0f);
  return f;
}
#endif
