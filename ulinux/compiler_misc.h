#ifndef ULINUX_COMPILER_MISC_H
#define ULINUX_COMPILER_MISC_H
/******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*----------------------------------------------------------------------------*/
#if __GNUC__
  #define unreachable __builtin_unreachable
  #define NOCLONE __attribute__((noclone))
  #define NOINLINE __attribute__((noinline))
#else
  #define unreachable()
  /*too risky to have the entry point optimized out with a new C toolchain*/
#endif
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#if __GNUC__
  #define PACKED __attribute__((packed))
#endif

#ifndef PACKED
  #error "the compiler must support packed structure"
#endif
/*----------------------------------------------------------------------------*/
#endif
