#ifndef SRC_PARSE_H
#define SRC_PARSE_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
s8 src_parse(u8 *src,s32 src_sz,u8 *src_pathname_default,struct i **is,
                                          s32 *is_last,struct msgs_ctx *msgs);
s8 is_unmap(struct i *is,s32 is_last,struct msgs_ctx *msgs);
#endif
