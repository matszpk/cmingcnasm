#ifndef SRC_PARSE_H
#define SRC_PARSE_H
//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
k_s8 src_parse(k_u8 *src,k_s32 src_sz,k_u8 *src_pathname_default,struct i **is,
                                          k_s32 *is_last,struct msgs_ctx *msgs);
k_s8 is_unmap(struct i *is,k_s32 is_last,struct msgs_ctx *msgs);
#endif
