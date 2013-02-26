#define SO_C
#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/utils/ascii/string/vsprintf.h>

#include "so.h"

#define POUT(f,...) u_a_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
                                __VA_ARGS__)
#define POUTC(s) {k_l rl;do{rl=sysc(write,3,1,s,sizeof(s));}\
                         while(rl==-K_EINTR||rl==-K_EAGAIN);}

k_i function_1(k_i a,k_i b)
{
  POUT("function_1:%d,%d\n",a,b);
  return 0;
}

k_i function_2(k_i a,k_i b)
{
  POUT("function_2:%d,%d,%d\n",a,b,a+b);
  return a-b;
}
