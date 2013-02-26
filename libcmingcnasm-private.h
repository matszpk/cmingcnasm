#ifndef CMINGCNASM_PRIVATE_H
#define CMINGCNASM_PRIVATE_H
//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define _(x) ((k_u8*)x)
#define cs_n(s) (sizeof(s)-1)

struct i_f{
  k_u8 f;
  k_u16 val;
};

#define FS_MAX 16
#define SRC_PATHNAME_SZ_MAX 255//0 terminating char not counted
struct i{
  k_u8 src_pathname[SRC_PATHNAME_SZ_MAX+1];
  k_s32 src_l;
  k_s32 l;

  k_u8* label_s;
  k_u8* label_e;

  struct i_mnemonic_map *map;

  struct i_f fs[FS_MAX];;
};
#endif
