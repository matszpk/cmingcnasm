#define SO_C
#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/utils/ascii/string/vsprintf.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define u8 ulinux_u8
//------------------------------------------------------------------------------

#include "so.h"

#define POUT(fmt,...) ulinux_dprintf(1,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
##__VA_ARGS__)

#define POUTC(str) {l rl;do{rl=ulinux_sysc(write,3,1,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

i function_1(i a,i b)
{
  POUT("function_1:%d,%d\n",a,b);
  return 0;
}

i function_2(i a,i b)
{
  POUT("function_2:%d,%d,%d\n",a,b,a+b);
  return a-b;
}
