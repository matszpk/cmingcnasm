#ifndef CMINGCNASM_PRIVATE_H
#define CMINGCNASM_PRIVATE_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define cs_n(s) (sizeof(s)-1)

struct i_f{
	u8 f;
	u16 val;
};

#define FS_MAX 16
#define SRC_PATHNAME_SZ_MAX 255/*0 terminating char not included*/
struct i{
	u8 src_pathname[SRC_PATHNAME_SZ_MAX+1];
	u64 src_l;
	u64 l;

	u8* label_s;
	u8* label_e;

	struct i_mnemonic_map *map;

	struct i_f fs[FS_MAX];;
};

#ifndef SRC_PARSE_C
extern void fs_val_parser_init(void);
#endif
#endif
