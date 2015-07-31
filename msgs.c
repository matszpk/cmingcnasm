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

#define CMINGCN_MSGS_ERR -2/*XXX:also defined in public headers*/

static u8 grow(struct msgs_ctx *msgs,s32 len)
{
	sl addr;
	s8 r;

	r=0;

	if(!*msgs->sz){/*first allocation, then mmapping*/
		addr=mmap((sl)len,PROT_READ|PROT_WRITE,MAP_PRIVATE
							|MAP_ANONYMOUS,-1);
		if(ISERR(addr)){
			r=CMINGCN_MSGS_ERR;
			goto exit;
		}
	}else{/*remapping*/
		if(*msgs->sz+len>msgs->sz_max){
			r=CMINGCN_MSGS_ERR;
			goto exit;
		}

		addr=mremap(*msgs->msgs,*msgs->sz,*msgs->sz+len,MREMAP_MAYMOVE);
		if(ISERR(addr)){
			r=CMINGCN_MSGS_ERR;
			goto exit;
		}
	}

	*msgs->msgs=(u8*)addr;
	*msgs->sz=*msgs->sz+len;
exit:
	return r; 
}

s8 msg_hidden(struct msgs_ctx *msgs,u8 *fmt,...)
{
	va_list args;
	u64 len;
	s32 r;

	if(!msgs->msgs||!msgs->sz) return 0;

	va_start(args,fmt);
	len=vsnprintf(0,0,fmt,args);/*compute needed space*/
	va_end(args);

	if(len==0) r=CMINGCN_MSGS_ERR;
	else{
		s32 old_sz=*msgs->sz;

		r=grow(msgs,(s32)(len+1));
		if(!r){
			s32 shift;

			va_start(args,fmt);
			shift=old_sz?1:0;
			len=vsnprintf(*msgs->msgs+old_sz-shift,len+1,fmt,args);
			va_end(args);
			if(len==0) r=CMINGCN_MSGS_ERR;
		}
	}
	return r;
}
