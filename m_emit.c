/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*------------------------------------------------------------------------------
compiler stuff
------------------------------------------------------------------------------*/
#include <stdarg.h>
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
ulinux stuff
------------------------------------------------------------------------------*/
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/endian.h>

#include "ulinux-namespace.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
public
------------------------------------------------------------------------------*/
#include "cmingcnasm.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
private
------------------------------------------------------------------------------*/
#include "msgs.h"
#include "is.h"
#include "libcmingcnasm-private.h"
/*----------------------------------------------------------------------------*/

static void bytes_new(u8 bytes,u8 **m,u64 *m_sz)
{
	u64 new_len;
	sl addr;

	if(!*m_sz){/*first allocation, then mmapping*/
		new_len=bytes;
		addr=mmap(bytes,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,
									-1);
	}else{/*remapping*/
		new_len=*m_sz+bytes;
		addr=mremap(*m,*m_sz,new_len,MREMAP_MAYMOVE);
	}

	*m=(u8*)addr;
	*m_sz=new_len;
}

static void le32_new(u8 **m,u64 *m_sz)
{
	bytes_new(sizeof(u32),m,m_sz);
}

static void le64_new(u8 **m,u64 *m_sz)
{
	bytes_new(sizeof(u64),m,m_sz);
}

static void emit_64(u64 m_i_le,u8 **m,u64 *m_sz)
{
	u64 *p;

	le64_new(m,m_sz);

	p=(u64*)(*m+*m_sz-sizeof(m_i_le));
	*p=m_i_le;
}

static void emit_32(u32 m_i_le,u8 **m,u64 *m_sz)
{
	u32 *p;

	le32_new(m,m_sz);

	p=(u32*)(*m+*m_sz-sizeof(m_i_le));
	*p=m_i_le;
}


static struct i_f *f_get(struct i *i,u8 f)
{
	struct i_f *r;
	u8 cur_f;

	cur_f=0;

	loop{
		if(cur_f>=FS_MAX) break;
		if(i->fs[cur_f].f==f) break;
		++cur_f;
	}
	r=0;
	if(cur_f<FS_MAX) r=&i->fs[cur_f];
	return r;
}

/*============================================================================*/
/*mubuf*/
static s8 mubuf_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_OFFSET:
		case F_OFFEN:
		case F_IDXEN:
		case F_GLC:
		case F_ADDR64:
		case F_LDS:
		case F_VADDR:
		case F_VDATA:
		case F_SRSRC:
		case F_SLC:
		case F_TFE:
		case F_SOFFSET:
			break;
		default:
			msg(msgs,"error:machine emit:mubuf:%s:%u:pp(%u):%s field illegal in this instruction\n",
						i->src_pathname,i->src_l,i->l,
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		}

		++i_f;
	}
	return 0;
}

#define MSG(x,y) msg(msgs,x ":machine emit:mubuf:%s:%u:pp(%u):" y,\
i->src_pathname,i->src_l,i->l)
static s8 mubuf(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	s8 r;
	u64 m_i;
	u8 offset_set;
	struct i_f *f;
	u8 vaddr_set;

	r=mubuf_fs_chk(i,msgs);
	if(r) return r;
  
	m_i=0;

	offset_set=0;
	f=f_get(i,F_OFFSET);
	if(f){
		offset_set=1;
		if(f->val&~0xfff){
			MSG("error","offset value too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=f->val&0xfff;
	}

	f=f_get(i,F_OFFEN);
	if(f) m_i|=(f->val&1)<<12;

	f=f_get(i,F_IDXEN);
	if(f) m_i|=(f->val&1)<<13;

	f=f_get(i,F_GLC);
	if(f) m_i|=(f->val&1)<<14;

	f=f_get(i,F_ADDR64);
	if(f) m_i|=(f->val&1)<<15;

	f=f_get(i,F_LDS);
	if(f) m_i|=(f->val&1)<<16;

	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_MUBUF))&0x7f)<<18;

	m_i|=(u64)0b111000UL<<26;

	vaddr_set=0;
	f=f_get(i,F_VADDR);
	/*may target 1 *or 2 consecutive* vgprs, that based on idxen and offen*/
	if(f){
		vaddr_set=1;
		m_i|=(u64)(f->val&0xff)<<32;
	}

	f=f_get(i,F_VDATA);
	if(f) m_i|=(u64)(f->val&0xff)<<40;
  
	f=f_get(i,F_SRSRC);
	if(f) m_i|=(u64)(f->val&0x1f)<<48;

	f=f_get(i,F_SLC);
	if(f) m_i|=(u64)(f->val&1)<<54;

	f=f_get(i,F_TFE);
	if(f) m_i|=(u64)(f->val&1)<<55;

	f=f_get(i,F_SOFFSET);
	if(f){/*may select a vgpr which would be out of range*/
		if(f->val&~0xff){
			MSG("error","soffset value targets a vgpr\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(u64)(f->val&0xff)<<56;
	}

/*------------------------------------------------------------------------------
start instruction scale checks (warnings and errors)
------------------------------------------------------------------------------*/
	/*if addr64==1, idxen and offen must be 0*/
	if(m_i>>15&1&&(m_i>>12&1||m_i>>13&1)){
		MSG("error","when addr64==1 then offen and idxen must be 0\n");
		return CMINGCNASM_ERR;
	}

	/*if offen=1 the content of a set offset will be ignored*/
	if(m_i>>12&1&&offset_set)
		MSG("warning","offen is 1 then content of offset will be ignored\n");

	/*if offen=0 and idxen=0, setting a value for vaddr in useless*/
	if(vaddr_set&&!(m_i>>12&1)&&!(m_i>>13&1))
		MSG("warning","setting a value to vaddr is useless unless you set offen or idxen\n");

	/*if offen=1 and idxen=1, better double check vaddr content*/
	if(m_i>>12&1&&m_i>>13&1)
		MSG("warning","offen and idxen set together, double check the content of the 2 vgprs pointed by vaddr\n");
 
	/*illegal to set tfe and lds together*/
	if(m_i>>55&1&&m_i>>16&1){
		MSG("error","cannot set tfe and lds together\n");
		return CMINGCNASM_ERR;
	}
/*----------------------------------------------------------------------------*/

	emit_64(cpu2le64(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end mubuf*/
/*============================================================================*/


/*============================================================================*/
/*s_waitcnt*/
static s8 s_waitcnt_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;
		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_VM_CNT:
		case F_EXP_CNT:
		case F_LGKM_CNT:
			break;
		case F_SIMM16:
			msg(msgs,"error:machine emit:s_waitcnt:%s:%u:pp(%u):don't use %s field, use vm_cnt exp_cnt lgkm_cnt sub-fields instead\n",
						i->src_pathname,i->src_l,i->l,
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		default:
			msg(msgs,"error:machine emit:s_waitcnt:%s:%u:pp(%u):%s field illegal in this instruction\n",
						i->src_pathname,i->src_l,i->l,
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		}

		++i_f;
	}
	return 0;
}

#define MSG(x) msg(msgs,"error:machine emit:s_waitcnt:%s:%u:pp(%u):" x,\
i->src_pathname,i->src_l,i->l)
static s8 s_waitcnt(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u32 m_i;
	struct i_f *f;
	s8 r;

	r=s_waitcnt_fs_chk(i,msgs);
	if(r) return r;

	m_i=0;

	f=f_get(i,F_VM_CNT);
	if(f){
		if(f->val>0xf){
			MSG("vector memory count too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=f->val&0xf;
	}

	f=f_get(i,F_EXP_CNT);
	if(f){
		if(f->val>7){
			MSG("vgpr export count too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(f->val&7)<<4;
	}

	f=f_get(i,F_LGKM_CNT);
	if(f){
		if(f->val>0x1f){
			MSG("LDS/GDS/Konstant/Message count too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(f->val&0x1f)<<8;
	}

	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_SOPP))&0x7f)<<16;

	m_i|=(u32)0b101111111UL<<23;

	emit_32(cpu2le32(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end s_waitcnt*/
/*============================================================================*/


/*============================================================================*/
/*s_endpgm*/
static s8 s_endpgm(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u32 m_i;

	if(i->fs[0].f!=F_INVALID){
		msg(msgs,"error:machine emit:s_endpgm:%s:%u:pp(%u):no field are allowed for this instruction\n",
						i->src_pathname,i->src_l,i->l);
		return CMINGCNASM_ERR;
	}

	m_i=0;
	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_SOPP))&0x7f)<<16;
	m_i|=(u32)0b101111111UL<<23;

	emit_32(cpu2le32(m_i),m,m_sz);
	return 0;
}
/*end s_endpgm*/
/*============================================================================*/

static s8 sopp(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	s8 r;

	r=0;

	switch(i->map->op_base){
	case 1:
		r=s_endpgm(i,m,m_sz,msgs);
		break;
	case 12:
		r=s_waitcnt(i,m,m_sz,msgs);
		break;
	default:
		msg(msgs,"error:machine emit:%s:%u:pp(%u):unknown sopp opcode %u\n",
                                 i->src_pathname,i->src_l,i->l,i->map->op_base);
		r=CMINGCNASM_ERR;
	}
	return r;
}


/*============================================================================*/
/*export*/
static s8 export_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_EN:
		case F_TGT:
		case F_COMPR:
		case F_DONE:
		case F_VM:
		case F_VSRC0:
		case F_VSRC1:
		case F_VSRC2:
		case F_VSRC3:
			break;
		default:
			msg(msgs,"error:machine emit:export:%s:%u:pp(%u):%s field illegal in this instruction\n",
						i->src_pathname,i->src_l,i->l,
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
    		}

		++i_f;
	}
	return 0;
}
#undef MSG

#define MSG(x,y) msg(msgs,x ":machine emit:export:%s:%u:pp(%u):" y,\
i->src_pathname,i->src_l,i->l)
static s8 export(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u64 m_i;
	struct i_f *f;
	u8 vsrc0_set;
	u8 vsrc1_set;
	u8 vsrc2_set;
	u8 vsrc3_set;
	s8 r;

	r=export_fs_chk(i,msgs);
	if(r) return r;

	m_i=0;

	f=f_get(i,F_EN);
	if(f){
		if(f->val>0xf){
			MSG("error","enable mask too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=f->val&0xf;
	}

	f=f_get(i,F_TGT);
	if(f) m_i|=(f->val&0x3f)<<4;

	f=f_get(i,F_COMPR);
	if(f) m_i|=(f->val&1)<<10;

	f=f_get(i,F_DONE);
	if(f) m_i|=(f->val&1)<<11;

	f=f_get(i,F_VM);
	if(f) m_i|=(f->val&1)<<12;

	m_i|=(u64)0b111110UL<<26;

	vsrc0_set=0;
	f=f_get(i,F_VSRC0);
	if(f){
		m_i|=(u64)(f->val&0xff)<<32;
		vsrc0_set=1;
	}
  
	vsrc1_set=0;
	f=f_get(i,F_VSRC1);
	if(f){
		m_i|=(u64)(f->val&0xff)<<40;
		vsrc1_set=1;
	}
  
	vsrc2_set=0;
	f=f_get(i,F_VSRC2);
	if(f){
		m_i|=(u64)(f->val&0xff)<<48;
		vsrc2_set=1;
	}

	vsrc3_set=0;
	f=f_get(i,F_VSRC3);
	if(f){
		m_i|=(u64)(f->val&0xff)<<56;
		vsrc3_set=1;
	}

/*------------------------------------------------------------------------------
start instruction scale checks (warnings and errors)
------------------------------------------------------------------------------*/
	/*useless to enable vsrc2 and vsrc3 when compr=1*/
	if(m_i>>10&1&&m_i&0x3)
		MSG("warning","compr is 1 then useless to enable vsrc2 and/or vsrc3\n");
	if(m_i&1&&!vsrc0_set)
		MSG("warning","vsrc0 export is enabled without vsrc0 field\n");
	if(!m_i&1&&vsrc0_set)
		MSG("warning","vsrc0 export is disabled but vsrc0 field is defined\n");
	if(m_i&1&&!vsrc1_set)
		MSG("warning","vsrc1 export is enabled without vsrc1 field\n");
	if(!m_i&1&&vsrc1_set)
		MSG("warning","vsrc1 export is disabled but vsrc1 field is defined\n");
	if(m_i&1&&!vsrc2_set)
		MSG("warning","vsrc2 export is enabled without vsrc2 field\n");
	if(!m_i&1&&vsrc2_set)
		MSG("warning","vsrc2 export is disabled but vsrc2 field is defined\n");
	if(m_i&1&&!vsrc3_set)
		MSG("warning","vsrc3 export is enabled without vsrc3 field\n");
	if(!m_i&1&&vsrc3_set)
		MSG("warning","vsrc3 export is disabled but vsrc3 field is defined\n");
/*----------------------------------------------------------------------------*/

	emit_64(cpu2le64(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end export*/
/*============================================================================*/


/*============================================================================*/
/*vop3a*/
#define MSG(x,...) msg(msgs,"error:machine emit:%s(vop3a):%s:%u:pp(%u):" x,\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static s8 vop3a_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_SRC0:
		case F_VDST:
		case F_SRC1:
		case F_SRC2:
		case F_OMOD:
		case F_NEG:
		case F_ABS:
		case F_CLAMP:
			break;
		default:
			MSG("%s field illegal in this instruction\n",
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		}
		++i_f;
	}
	return 0;
}

static s8 vop3a(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u64 m_i;
	struct i_f *f;
	s8 r;

	r=vop3a_fs_chk(i,msgs);
	if(r) return r;

	m_i=0;

	f=f_get(i,F_VDST);
	if(f) m_i|=f->val&0xff;

	f=f_get(i,F_ABS);
	if(f){
		if(f->val>3){
			MSG("vsrc index for absolute value above 3\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(f->val&3)<<8;
	}

	f=f_get(i,F_CLAMP);
	if(f) m_i|=(f->val&1)<<11;

	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP3A))&0x1ff)
									<<17;

	m_i|=(u64)0b110100UL<<26;

	f=f_get(i,F_SRC0);
	if(f) m_i|=(u64)(f->val&0x1ff)<<32;

	f=f_get(i,F_SRC1);
	if(f) m_i|=(u64)(f->val&0x1ff)<<41;

	f=f_get(i,F_SRC2);
	if(f) m_i|=(u64)(f->val&0x1ff)<<50;

	f=f_get(i,F_OMOD);
	if(f){
		if(f->val>3){
			MSG("output modifier above 3\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(u64)(f->val&3)<<59;
	}

	f=f_get(i,F_NEG);
	if(f){
		if(f->val>3){
			MSG("vsrc index for negation value above 3\n");
			return CMINGCNASM_ERR;
		}
		m_i|=(u64)(f->val&3)<<61;
	}

	emit_64(cpu2le64(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end of vop3a*/
/*============================================================================*/


/*============================================================================*/
/*vop1*/
#define MSG(x,...) msg(msgs,"error:machine emit:%s(vop1):%s:%u:pp(%u):" x,\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static s8 vop1_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_SRC0:
		case F_VDST:
			break;
		default:
			MSG("%s field illegal in this instruction\n",
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		}
		++i_f;
	}
	return 0;
}

static s8 vop1(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u32 m_i;
	struct i_f *f;
	s8 r;

	r=vop1_fs_chk(i,msgs);
	if(r) return r;

	m_i=0;

	f=f_get(i,F_SRC0);
	if(f){
		if(f->val>0x1ff){
			MSG("src0 value too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=f->val&0x1ff;
	}

	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP1))&0xff)<<9;

	f=f_get(i,F_VDST);
	if(f) m_i|=(f->val&0xff)<<17;

	m_i|=(u32)0b0111111UL<<25;

	emit_32(cpu2le32(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end of vop1*/
/*============================================================================*/


/*============================================================================*/
/*vop2*/
#define MSG(x,...) msg(msgs,"error:machine emit:%s(vop2):%s:%u:pp(%u):" x,\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static s8 vop2_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
	u8 i_f;

	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

 		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_SRC0:
		case F_VSRC1:
		case F_VDST:
			break;
		default:
			MSG("%s field illegal in this instruction\n",
						fs_mnemonic[i->fs[i_f].f]);
			return CMINGCNASM_ERR;
		}
		++i_f;
	}
	return 0;
}

static s8 vop2(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u32 m_i;
	struct i_f *f;
	s8 r;

	r=vop2_fs_chk(i,msgs);
	if(r) return r;

	m_i=0;

	f=f_get(i,F_SRC0);
	if(f){
		if(f->val>0x1ff){
			MSG("src0 value too large\n");
			return CMINGCNASM_ERR;
		}
		m_i|=f->val&0x1ff;
	}

	f=f_get(i,F_VSRC1);
	if(f) m_i|=(f->val&0xff)<<9;

	f=f_get(i,F_VDST);
	if(f) m_i|=(f->val&0xff)<<17;

	m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP2))&0x3f)<<25;

	m_i|=(u32)0b0UL<<31;

	emit_32(cpu2le32(m_i),m,m_sz);
	return 0;
}
#undef MSG
/*end of vop2*/
/*============================================================================*/


/*============================================================================*/
/*vop1|vop3a*/
#define VOP1  0
#define VOP3A 1
static s8 vop1_vop3a_select(struct i *i)
{
	s8 r;
	u8 i_f;

	r=VOP1;
	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_SRC0:
		case F_VDST:
			break;
		case F_SRC1:
		case F_SRC2:
		case F_OMOD:
		case F_NEG:
		case F_ABS:
		case F_CLAMP:
			r=VOP3A;
			break;
		}

		if(r!=VOP1) break;
		++i_f;
	}
	return r;
}

static s8 vop1_vop3a(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	s8 r;

	r=vop1_vop3a_select(i);

	switch(r){
	case VOP1:
		r=vop1(i,m,m_sz,msgs);
		break;
	case VOP3A:
		r=vop3a(i,m,m_sz,msgs);
		break;
	}   
	return r;
}
/*end of vop1|vop3a*/
/*============================================================================*/


/*============================================================================*/
/*vop2|vop3a*/
#define VOP2  0
static s8 vop2_vop3a_select(struct i *i)
{
	s8 r;
	u8 i_f;

	r=VOP2;
	i_f=0;

	loop{
		if(i_f>=FS_MAX) break;

		if(i->fs[i_f].f==F_INVALID) break;

		switch(i->fs[i_f].f){
		case F_SRC0:
		case F_VSRC1:
		case F_VDST:
			break;
		case F_SRC1:
		case F_SRC2:
		case F_OMOD:
		case F_NEG:
		case F_ABS:
		case F_CLAMP:
			r=VOP3A;
			break;
		}

		if(r!=VOP2) break;
		++i_f;
	}
	return r;
}

static s8 vop2_vop3a(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	s8 r;

	r=vop2_vop3a_select(i);

	switch(r){
	case VOP2:
		r=vop2(i,m,m_sz,msgs);
		break;
	case VOP3A:
		r=vop3a(i,m,m_sz,msgs);
		break;
	}   
	return r;
}
/*end of vop1|vop3a*/
/*============================================================================*/


static s8 i_encode(struct i *i,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	s8 r;

	switch(i->map->fmts){
	case BIT(FMT_MUBUF):
		r=mubuf(i,m,m_sz,msgs);
		break;
	case BIT(FMT_SOPP):
		r=sopp(i,m,m_sz,msgs);
		break;
	case BIT(FMT_EXP):
		r=export(i,m,m_sz,msgs);
		break;
	case BIT(FMT_VOP1)|BIT(FMT_VOP3A):
		r=vop1_vop3a(i,m,m_sz,msgs);
		break;
	case BIT(FMT_VOP2)|BIT(FMT_VOP3A):
		r=vop2_vop3a(i,m,m_sz,msgs);
		break;
	default:
		msg(msgs,"error:machine emit:%s:%u:pp(%u):instruction encoder not supported 0x%08x\n",
				i->src_pathname,i->src_l,i->l,i->map->fmts);
		r=CMINGCNASM_ERR;
	}
	return r;
}

s8 m_emit(struct i *is,u64 is_n,u8 **m,u64 *m_sz,struct msgs_ctx *msgs)
{
	u64 i;
	s8 r;

	r=0;

	*m=0;
	*m_sz=0;
	i=0;
	loop{
		if(i>=is_n) break;

		r=i_encode(&is[i],m,m_sz,msgs);
		if(r!=0) break;
		++i;
	}
 
	if(r!=0&&(*m_sz)) munmap(*m,*m_sz);
	return r;
}
