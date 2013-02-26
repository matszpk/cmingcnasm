//******************************************************************************
//this code is protected by the GNU affero GPLv3
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//------------------------------------------------------------------------------
//compiler stuff
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//ulinux stuff
//------------------------------------------------------------------------------
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>

#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/mem.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//used libs
//------------------------------------------------------------------------------
#include <cmingcndis.h>
//------------------------------------------------------------------------------

#ifndef QUIET
k_u8 *g_dprintf_buf;
#define DPRINTF_BUF_SZ 2048
#define PERR(f,...) u_a_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
                               __VA_ARGS__)
#define PERRC(s) sysc(write,3,2,s,sizeof(s))
#endif

//******************************************************************************
void start(k_i argc, k_u8 **argv_envp)
{
#ifndef QUIET 
  static k_u8 dprintf_buf[DPRINTF_BUF_SZ];
  g_dprintf_buf=dprintf_buf;
#endif
  (void)argc;(void)argv_envp;
  k_l r=cmingcndis_dis(0,0);
  if(K_ISERR(r)) PERRC("boom!\n");
  else PERRC("yeay!\n");
  sysc(exit_group,1,0);
}
