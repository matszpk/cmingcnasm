#ifndef SRC_PARSE_H
#define SRC_PARSE_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
s8 src_parse(u8 *src,u64 src_sz,u8 *src_pathname_default,struct i **is,
					u64 *is_n,struct msgs_ctx *msgs);
#endif
