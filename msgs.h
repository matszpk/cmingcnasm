#ifndef MSGS_H
#define MSGS_H
//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct msgs_ctx{
  k_s32 sz_max;
  k_u8 **msgs;
  k_s32 *sz;
};

k_s8 msg(struct msgs_ctx *msgs,k_u8 *fmt,...);
#endif
