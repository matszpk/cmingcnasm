/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*------------------------------------------------------------------------------
compiler stuff
------------------------------------------------------------------------------*/
#include <stdarg.h>
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
ulinux stuff
------------------------------------------------------------------------------*/
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#include "ulinux-namespace.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
private
------------------------------------------------------------------------------*/
#include "msgs.h"
/*----------------------------------------------------------------------------*/

static void grow(struct msgs_ctx *msgs,u64 sz)
{
	sl addr;

	if(!*msgs->sz)/*first allocation, then mmapping*/
		addr=mmap(sz,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,
									-1);
	else addr=mremap(*msgs->msgs,*msgs->sz,*msgs->sz+sz,MREMAP_MAYMOVE);

	*msgs->msgs=(u8*)addr;
	*msgs->sz=*msgs->sz+sz;
}

void msg_hidden(struct msgs_ctx *msgs,u8 *fmt,...)
{
	va_list args;
	u64 str_sz;
	u64 old_msg_sz;

	if(!msgs->msgs||!msgs->sz) return;

	va_start(args,fmt);
	str_sz=vsnprintf(0,0,fmt,args);/*compute needed space*/
	va_end(args);

	old_msg_sz=*msgs->sz;
	grow(msgs,str_sz);

	va_start(args,fmt);
	vsnprintf(*msgs->msgs+old_msg_sz,str_sz,fmt,args);
	va_end(args);
}
