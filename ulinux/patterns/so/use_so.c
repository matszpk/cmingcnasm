#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define u8 ulinux_u8
#define exit(a) ulinux_sysc(exit,1,a)
//------------------------------------------------------------------------------


#include "so.h"

void _start(void)
{
  u8 buf[DPRINTF_BUF_SZ];
  dprintf_buf=&buf[0];

  function_1(1,2);
  function_2(3,4);
  exit(0);
}
