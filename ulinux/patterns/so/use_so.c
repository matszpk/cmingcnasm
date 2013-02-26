#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>

#include "so.h"

void _start(void)
{
  k_u8 _dprintf_buf[DPRINTF_BUF_SZ];
  dprintf_buf=&_dprintf_buf[0];

  function_1(1,2);
  function_2(3,4);
  sysc(exit_group,1,0);
}
