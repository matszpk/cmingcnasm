#ifndef ULINUX_TYPES_H
#define ULINUX_TYPES_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#include <ulinux/arch/types.h>

typedef k_t  k_s8;
typedef k_ut k_u8;

typedef k_s  k_s16;
typedef k_us k_u16;

typedef k_i  k_s32;
typedef k_u  k_u32;

typedef k_ll   k_s64;
typedef k_ull  k_u64;

#if BITS_PER_LONG==64
typedef k_ul  k_sz;
typedef k_l k_ptrdiff;
#else
typedef k_u  k_sz;
typedef k_i k_ptrdiff;
#endif
#endif

