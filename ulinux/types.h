#ifndef ULINUX_TYPES_H
#define ULINUX_TYPES_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#include <ulinux/arch/types.h>

#define ulinux_s8 ulinux_sc
#define ulinux_u8 ulinux_uc

#define ulinux_s16 ulinux_ss
#define ulinux_u16 ulinux_us

#define ulinux_s32 ulinux_si
#define ulinux_u32 ulinux_ui
#define ulinux_f32 ulinux_f

#define ulinux_s64 ulinux_sll
#define ulinux_u64 ulinux_ull

#if BITS_PER_LONG==64
#  define ulinux_sz ulinux_ul
#  define ulinux_ptrdiff ulinux_sl
#else
#  define ulinux_sz ulinux_ui
#  define ulinux_ptrdiff ulinux_si
#endif
#endif
