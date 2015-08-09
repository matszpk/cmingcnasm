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
s8 cmingcndis_dis(u8 *m,/*(in)pointer on machine code*/
                  u64 m_sz,/*(in)machine code size*/
                  u8 **src,/*(out)utf-8 source code buffer*/
                  u64 *src_sz,/*(out)mmaped source code size buffer size*/
                  u8 **msgs,/*(out)message buffer(disabled if 0)*/
                  u64 *msgs_sz);/*(out)mmaped message buffer size*/
#endif
