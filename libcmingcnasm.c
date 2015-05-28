/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/


/***IMPORTANT NOTICE: do *NOT* factor the code. It still misses features***/


/*------------------------------------------------------------------------------
compiler stuff
------------------------------------------------------------------------------*/
#include <stdarg.h>
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
ulinux stuff
------------------------------------------------------------------------------*/
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>

#include <ulinux/utils/ascii/string/vsprintf.h>

#include "ulinux-namespace.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
public
------------------------------------------------------------------------------*/
#include "cmingcnasm.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
private
------------------------------------------------------------------------------*/
#include "msgs.h"
#define IS_C
#include "is.h"
#undef IS_C
#include "libcmingcnasm-private.h"
#include "src_parse.h"
#include "m_emit.h"
/*----------------------------------------------------------------------------*/

s8 cmingcnasm_asm(u8 *src,
                  s32 src_sz,
                  u8 *src_pathname_default,
                  s32 m_sz_max,
                  u8 **m,
                  s32 *m_sz,
                  s32 msgs_sz_max,
                  u8 **msgs,
                  s32 *msgs_sz)
{
	s8 r0;
	s8 r1;
	struct msgs_ctx msgs_c;
	struct i *is;
	s32 is_last;

	if(msgs&&msgs_sz){
		*msgs=0;
		*msgs_sz=0;
	}

	msgs_c.sz_max=msgs_sz_max;
	msgs_c.msgs=msgs;
	msgs_c.sz=msgs_sz;

	if(!src||src_sz<=0){
		r0=msg(&msgs_c,"source code empty(p=0x%p:sz=%d)\n",src,src_sz);
		if(!r0) r0=CMINGCNASM_ERR;
		goto exit;
	}
	if(m_sz_max<=0){
		r0=msg(&msgs_c,"invalid maximum size of machine code(%d)\n",
								m_sz_max);
		if(!r0) r0=CMINGCNASM_ERR;
		goto exit;
	}

	/*work starts here*/
	r0=src_parse(src,src_sz,src_pathname_default,&is,&is_last,&msgs_c);
	if(r0!=0) goto exit;

	r0=m_emit(is,is_last,m_sz_max,m,m_sz,&msgs_c);
	if(r0!=0) goto err_unmap_is;

	goto exit;

err_unmap_is:
	r1=is_unmap(is,is_last,&msgs_c);
	if(r1) r0=r1;

exit:
	return r0;
}
