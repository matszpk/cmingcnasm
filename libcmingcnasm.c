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
#include "is.h"
#include "libcmingcnasm-private.h"
#include "src_parse.h"
#include "m_emit.h"
/*----------------------------------------------------------------------------*/

/*we don't want to rely on the elf loader for that*/
void cmingcnasm_static_init(void)
{
	i_mnemonic_maps_init();
	fs_mnemonic_init();
	fs_val_parser_init();
}

s8 cmingcnasm_asm(u8 *src,
                  u64 src_sz,
                  u8 *src_pathname_default,
                  u8 **m,
                  u64 *m_sz,
                  u8 **msgs,
                  u64 *msgs_sz)
{
	s8 r;
	struct msgs_ctx msgs_c;
	struct i *is;
	u64 is_n;

	if(msgs&&msgs_sz){
		*msgs=0;
		*msgs_sz=0;
	}

	msgs_c.msgs=msgs;
	msgs_c.sz=msgs_sz;

	if(!src||src_sz==0){
		msg(&msgs_c,"source code empty(p=0x%p:sz=%d)\n",src,src_sz);
		return CMINGCNASM_ERR;
	}

	/*work starts here*/
	r=src_parse(src,src_sz,src_pathname_default,&is,&is_n,&msgs_c);
	if(r!=0) goto exit;

	r=m_emit(is,is_n,m,m_sz,&msgs_c);
	if(r!=0) goto err_unmap_is;

	goto exit;

err_unmap_is:
	if(is_n) munmap(is,sizeof(struct i)*is_n);

exit:
	return r;
}
