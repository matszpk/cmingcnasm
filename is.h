#ifndef IS_H
#define IS_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/

#define FMT_SOP2   0
#define FMT_SOPK   1
#define FMT_SOP1   2
#define FMT_SOPC   3
#define FMT_SOPP   4
#define FMT_SMRD   5
#define FMT_VOP2   6
#define FMT_VOP1   7
#define FMT_VOPC   8
#define FMT_VOP3A  9
#define FMT_VOP3B  10
#define FMT_VINTRP 11
#define FMT_DS     12
#define FMT_MUBUF  13
#define FMT_MTBUF  14
#define FMT_MIMG   15
#define FMT_EXP    16

struct i_mnemonic_map{
	u8 *mnemonic;
	u32 fmts;
	/*base to compute operation codes fit to the different formats*/
	s16 op_base;
};

#ifndef BIT
#define BIT(x) (1<<(x))
#endif

/*instruction fields*/
#define F_SSRC0    0
#define F_SSRC1    1
#define F_SDST     2
#define F_SIMM16   3
#define F_OFFSET   4
#define F_IMM      5
#define F_SBASE    6
#define F_SRC0     7
#define F_SRC1     8
#define F_SRC2     9
#define F_VSRC0    10
#define F_VSRC1    11
#define F_VSRC2    12
#define F_VSRC3    13
#define F_VDST     14
#define F_ABS      15
#define F_CLAMP    16
#define F_OMOD     17
#define F_NEG      18
#define F_VSRC     19
#define F_ATTRCHAN 20
#define F_ATTR     21
#define F_OFFSET0  22
#define F_OFFSET1  23
#define F_GDS      24
#define F_ADDR     25
#define F_DATA0    26
#define F_DATA1    27
#define F_OFFEN    28
#define F_IDXEN    29
#define F_GLC      30
#define F_ADDR64   31
#define F_LDS      32
#define F_VADDR    33
#define F_VDATA    34
#define F_SRSRC    35
#define F_SLC      36
#define F_TFE      37
#define F_SOFFSET  38
#define F_DFMT     39
#define F_NFMT     40
#define F_DMASK    41
#define F_UNORM    42
#define F_DA       43
#define F_R128     44
#define F_LWE      45
#define F_SSAMP    46
#define F_EN       47
#define F_TGT      48
#define F_COMPR    49
#define F_DONE     50
#define F_VM       51
#define F_VM_CNT   52/*virtual field for s_waitcnt, actually in F_SIMM16*/
#define F_EXP_CNT  53/*virtual field for s_waitcnt, actually in F_SIMM16*/
#define F_LGKM_CNT 54/*virtual field for s_waitcnt, actually in F_SIMM16*/
#define F_INVALID  55

static inline u16 fmt_op_offset(u32 fmts,u32 fmt)
{
  if(fmts==(BIT(FMT_VOP1)|BIT(FMT_VOP3A))&&fmt==FMT_VOP3A) return 384;
  if(fmts==(BIT(FMT_VOP2)|BIT(FMT_VOP3A))&&fmt==FMT_VOP3A) return 256;
  return 0;
}

#define I_MNEMONIC_MAPS_N 813

#ifdef IS_C
struct i_mnemonic_map i_mnemonic_maps[I_MNEMONIC_MAPS_N];
u8 *fs_mnemonic[F_INVALID];
#else
extern struct i_mnemonic_map i_mnemonic_maps[I_MNEMONIC_MAPS_N];
extern void i_mnemonic_maps_init(void);
extern u8 *fs_mnemonic[F_INVALID];
extern void fs_mnemonic_init(void);
#endif
#endif
