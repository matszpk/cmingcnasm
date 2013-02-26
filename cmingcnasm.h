#ifndef CMINGCNASM_H
#define CMINGCNASM_H
//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define CMINGCNASM_ERR      -1
#define CMINGCNASM_MSGS_ERR -2//something went wrong with the message system
k_s8 cmingcnasm_asm(k_u8 *src,//(in)pointer on utf-8 source
                    k_s32 src_sz,//(in)source size in bytes
                    k_u8  *src_pathname_default,//(in)default source pathname
                    k_s32 m_sz_max,//(in)maximum size for machine code buffer
                    void **m,//(out)machine code buffer
                    k_s32 *m_sz,//(out)mmaped machine code buffer size 
                    k_s32 msgs_sz_max,//(in)maximum size for message buffer
                    k_u8 **msgs,//(out)message buffer(disabled if 0)
                    k_s32 *msgs_sz);//(out)mmaped message buffer size
#endif
