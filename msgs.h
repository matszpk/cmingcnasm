#ifndef MSGS_H
#define MSGS_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
struct msgs_ctx{
	s32 sz_max;
	u8 **msgs;
	s32 *sz;
};

#define msg(a,b,...) msg_hidden(a,(u8*)(b),##__VA_ARGS__)
s8 msg_hidden(struct msgs_ctx *msgs,u8 *fmt,...);
#endif
