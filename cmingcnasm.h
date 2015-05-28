#ifndef CMINGCNASM_H
#define CMINGCNASM_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define CMINGCNASM_ERR   -1
#define CMINGCN_MSGS_ERR -2/*something went wrong with the message system*/
/* map properly the type based on your userland abstraction (posix/ulinux...)*/
s8 cmingcnasm_asm(u8 *src,/*(in)pointer on utf-8 source code*/
                  s32 src_sz,/*(in)source code size in bytes*/
                  u8  *src_pathname_default,/*(in)default source pathname*/
                  s32 m_sz_max,/*(in)maximum size for machine code buffer*/
                  u8 **m,/*(out)machine code buffer*/
                  s32 *m_sz,/*(out)mmaped machine code buffer size*/
                  s32 msgs_sz_max,/*(in)maximum size for message buffer*/
                  u8 **msgs,/*(out)message buffer(disabled if 0)*/
                  s32 *msgs_sz);/*(out)mmaped message buffer size*/
#endif
