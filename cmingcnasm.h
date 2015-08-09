#ifndef CMINGCNASM_H
#define CMINGCNASM_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*we don't rely on the elf loader to build our static data*/
void cmingcnasm_static_init(void);

#define CMINGCNASM_ERR   -1
/* map properly the type based on your userland abstraction (posix/ulinux...)*/
s8 cmingcnasm_asm(u8 *src,/*(in)pointer on utf-8 source code*/
                  u64 src_sz,/*(in)source code size in bytes*/
                  u8 *src_pathname_default,/*(in)default source pathname*/
                  u8 **m,/*(out)machine code buffer*/
                  u64 *m_sz,/*(out)mmaped machine code buffer size*/
                  u8 **msgs,/*(out)message buffer(disabled if 0)*/
                  u64 *msgs_sz);/*(out)mmaped message buffer size*/
#endif
