#ifndef ULINUX_COMPILER_TYPES_H
#define ULINUX_COMPILER_TYPES_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//with GCC
//'char' is not a 'signed char' neither it is a 'unsigned char'
//but 'char' is signed on x86 and unsigned on ARM... feel the difference
typedef signed char        k_t;//t stands for tiny
typedef short              k_s;
typedef int                k_i;
typedef long               k_l;
typedef long long          k_ll;

typedef unsigned char      k_ut;//t stands for tiny
typedef unsigned short     k_us;
typedef unsigned           k_u;
typedef unsigned long      k_ul;
typedef unsigned long long k_ull;
#endif
