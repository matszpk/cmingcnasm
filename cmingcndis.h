#ifndef CMINGCNDIS_H
#define CMINGCNDIS_H
//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define CMINGCNDIS_ERR      -1
#define CMINGCNDIS_MSGS_ERR -2//something went wrong with the message system
k_s8 cmingcndis_dis(void *m,//(in)pointer on machine code
                    k_s32 m_sz,//(in)machine code size 
                    k_s32 src_sz_max,//(in)maximum size for source code buffer
                    k_u8 **src,//(out)utf-8 source code buffer
                    k_s32 *src_sz,//(out)mmaped source code size buffer size
                    k_s32 msgs_sz_max,//(in)maximum size for message buffer
                    k_u8 **msgs,//(out)message buffer(disabled if 0)
                    k_s32 *msgs_sz);//(out)mmaped message buffer size
#endif
