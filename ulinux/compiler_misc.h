#ifndef ULINUX_COMPILER_MISC_H
#define ULINUX_COMPILER_MISC_H
/******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#if GCC_VERSION >= 40500
  #define unreachable() __builtin_unreachable()
#else
  #define unreachable()
#endif
#endif
