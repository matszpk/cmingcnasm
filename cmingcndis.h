#ifndef CMINGCNDIS_H
#define CMINGCNDIS_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*we don't rely on the elf loader to build our static data*/
void cmingcndis_static_init(void);

#define CMINGCNDIS_ERR   -1
#define CMINGCN_MSGS_ERR -2/*something went wrong with the message system*/
s8 cmingcndis_dis(u8 *m,/*(in)pointer on machine code*/
                  s32 m_sz,/*(in)machine code size*/
                  s32 src_sz_max,/*(in)maximum size for source code buffer*/
                  u8 **src,/*(out)utf-8 source code buffer*/
                  s32 *src_sz,/*(out)mmaped source code size buffer size*/
                  s32 msgs_sz_max,/*(in)maximum size for message buffer*/
                  u8 **msgs,/*(out)message buffer(disabled if 0)*/
                  s32 *msgs_sz);/*(out)mmaped message buffer size*/
#endif
