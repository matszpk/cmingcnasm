/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/


/***IMPORTANT NOTICE: do *NOT* factor the code. It still misses features.***/


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

#include <ulinux/utils/mem.h>
#include <ulinux/utils/endian.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>

#include "ulinux-namespace.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
public
------------------------------------------------------------------------------*/
#include "cmingcndis.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
private
------------------------------------------------------------------------------*/
#include "msgs.h"
#include "is.h"
/*----------------------------------------------------------------------------*/

#define LABEL_I_FMT "l%05u:"
#define LABEL_I_TMPL "lXXXXX:"
#define LABEL_I_SZ_0 sizeof(LABEL_I_TMPL)
#define LABEL_I_SZ (LABEL_I_SZ_0-1)
#define LABEL_FMT "l%05u"
#define LABEL_TMPL "lXXXXX"
#define LABEL_SZ_0 sizeof(LABEL_TMPL)
#define LABEL_SZ (LABEL_SZ_0-1)
#define LABEL_OUT_OF_RANGE "**below machine code range**"
#define LABEL_NOT_DEFINED "**not defined**"

struct il{
	u8 *i;		/*location in machine code*/
	u8 *src;	/*location in src code*/
	s16 label;	/*>=0 means there is a label*/
};

struct label_pending{
	u8 *i;		/*location in machine code*/
	s16 label;	/*label value*/
};

struct ctx{
	u8 *m;/*start of machine code*/
	u8 *i;/*current machine instruction pointer*/
	
	u8 **src;
	u64 *src_sz;

	/*book keeping of instruction locations for label processing*/
	struct il *ils;
	u64 ils_n;
	s16 next_label;/*signed because -1 means no label for i location*/
	struct label_pending *labels_pending;
	u64 labels_pending_n;
	
	struct msgs_ctx *msgs;/*XXX:should be a log function*/
};

static void slab_grow(u8 **p,u64 old_sz,u64 sz_grow)
{
	sl addr;

	if(old_sz==0)/*first allocation, then mmapping*/
		addr=mmap(sz_grow,PROT_READ|PROT_WRITE,MAP_PRIVATE
							|MAP_ANONYMOUS,-1);
	else{
		u64 new_sz;

		new_sz=old_sz+sz_grow;
		addr=mremap(*p,old_sz,new_sz,MREMAP_MAYMOVE);
	}
	*p=(u8*)addr;
}

static struct il *ils_mark(struct ctx *c,u8 *i,u8 *src)
{
	struct il *il;
	
	slab_grow((u8**)&c->ils,c->ils_n*sizeof(*il),sizeof(*il));

	il=&c->ils[c->ils_n];
	++c->ils_n;

	il->i=i;
	il->src=src;
	il->label=-1;/*no label*/
	return il;
}

static void ils_unmap(struct ctx *c)
{
	if(c->ils_n==0) return;
	munmap(c->ils,sizeof(struct il)*c->ils_n);
}

static void labels_pending_new(struct ctx *c,u8 *target_i,s16 label)
{
	struct label_pending *label_pending;

	slab_grow((u8**)&c->labels_pending,c->labels_pending_n
				*sizeof(*label_pending),sizeof(*label_pending));

	label_pending=&c->labels_pending[c->labels_pending_n];
	++c->labels_pending_n;

	label_pending->i=target_i;
	label_pending->label=label;
}

static void labels_pending_unmap(struct ctx *c)
{
	if(c->labels_pending_n==0) return;
	munmap(c->labels_pending,sizeof(struct label_pending)
							*c->labels_pending_n);
}

static struct label_pending *labels_pending_find(struct ctx *c,u8 *i)
{
	u64 label_pending_idx;

	label_pending_idx=0;
	loop{
		struct label_pending *label_pending;
		
		if(label_pending_idx==c->labels_pending_n) break;
		
		label_pending=&c->labels_pending[label_pending_idx++];
		if(label_pending->i==i) return label_pending;
	}
	return 0;
}

static void label_pending_insert(struct ctx *c,struct il *il)
{
	struct label_pending *label_pending;

	label_pending=labels_pending_find(c,il->i);
	if(!label_pending) return;

	il->label=label_pending->label;
	snprintf(il->src,LABEL_I_SZ,LABEL_I_FMT,il->label);
	/*we don't cleanup the pending labels array*/
}

static void src_grow(struct ctx *c,u64 sz)
{
	slab_grow(c->src,*c->src_sz,sz);
	*c->src_sz+=sz;
}

#define src_out(a,b,...) src_out_hidden(a,(u8*)b,##__VA_ARGS__)
static void src_out_hidden(struct ctx *c,u8 *fmt,...)
{
	u64 sz;
	va_list args;

	va_start(args,fmt);
	sz=vsnprintf(0,0,fmt,args);
	va_end(args);

	src_grow(c,sz);

	va_start(args,fmt);
	vsnprintf(&(*c->src)[*c->src_sz-sz],sz,fmt,args);
	va_end(args);
}


#define i_mnemonic_map_find(a,b,c,d) i_mnemonic_map_find_hidden(a,b,c,(u8*)d)
static struct i_mnemonic_map *i_mnemonic_map_find_hidden(struct ctx *c,u8 fmt,
							s16 op,u8 *msg_str)
{
	struct i_mnemonic_map *map;

	map=&i_mnemonic_maps[0];

	loop{
		if(map->mnemonic==0) break;

		if(map->fmts&BIT(fmt)&&(map->op_base
					+fmt_op_offset(map->fmts,fmt))==op)
			return map;
		++map;
	}
	msg(c->msgs,"error:0x%lx:%s mnemonic not found\n",c->i-c->m,msg_str);
	return 0;
}

static void src_unmap(struct ctx *c)
{
	if(*c->src_sz==0) return;
	munmap(*c->src,*c->src_sz);
}

static void sopp_branch_above(struct ctx *c,s16 simm16,u8 *target_label)
{
	u32 *target_i;
	struct label_pending *label_pending;

	target_i=(u32*)c->i+1+simm16;

	label_pending=labels_pending_find(c,(u8*)target_i);
	if(label_pending){
		snprintf(target_label,LABEL_SZ_0,LABEL_FMT,
							label_pending->label);
		return;
	}

	snprintf(target_label,LABEL_SZ_0,LABEL_FMT,c->next_label);
	labels_pending_new(c,(u8*)target_i,c->next_label++);
}

static void sopp_branch_below_inrange(struct ctx *c,u8 *target_i,
							u8 *target_label)
{
	u64 il_idx;
	struct il *il;

	/*lookup for the target instruction location*/
	il_idx=0;
	il=0;
	loop{
		if(il_idx==c->ils_n) break;

		il=&c->ils[il_idx];
		if(target_i==il->i) break;
		++il_idx;
	}

	if(il_idx==c->ils_n){
		msg(c->msgs,"warning:0x%lx:sopp branch:target has no matching previous instruction\n",
								c->i-c->m);
		return;
	}

	if(il->label==-1){/*instruction has no label,insert it in src code*/
		il->label=c->next_label++;
		snprintf(il->src,LABEL_I_SZ,LABEL_I_FMT,il->label);
	}

	snprintf(target_label,LABEL_SZ_0,LABEL_FMT,il->label);
}

static void sopp_branch_below(struct ctx *c,s16 simm16,u8 *target_label)
{
	u32 *target_i;

	target_i=(u32*)c->i+1+simm16;
	if(target_i<(u32*)c->m){/*target machine instruction is out of range*/
		strcpy(&target_label[0],LABEL_OUT_OF_RANGE);
		return;
	}
	sopp_branch_below_inrange(c,(u8*)target_i,target_label);
}

static void sopp_branch(struct ctx *c,u32 i,struct i_mnemonic_map *map)
{
	u8 target_label[sizeof(LABEL_OUT_OF_RANGE)];/*0 terminated mandatory*/
	s16 simm16;

	strcpy(&target_label[0],LABEL_NOT_DEFINED);
	simm16=(s16)(i&0x0000ffff);

	if(simm16<0) sopp_branch_below(c,simm16,&target_label[0]);
	else sopp_branch_above(c,simm16,&target_label[0]);

	src_out(c,"        0x%08x         %s %s=%d(%s)\n",
						i,map->mnemonic,
						fs_mnemonic[F_SIMM16],simm16,
						target_label);
}

#define S_BRANCH		2
#define S_CBRANCH_EXECNZ	9
static s8 sopp(struct ctx *c)
{
	u32 i;
	u8 op;
	struct i_mnemonic_map *map;
	struct il* il;

	i=le322cpu(*(u32*)(c->i));
	op=(i>>16)&0x7f;

	map=i_mnemonic_map_find(c,FMT_SOPP,(s16)op,"sopp");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	if(op==12){
		/*the s_waitcnt sopp has 3 subfields instead af 1 simm16 field*/
		u8 vm_cnt;
		u8 exp_cnt;
		u8 lgkm_cnt;

		vm_cnt=i&0xf;
		exp_cnt=(i>>4)&0x7;
		lgkm_cnt=(i>>8)&0x1f;
		src_out(c,"        0x%08x         %s %s=%u %s=%u %s=%u\n",
					i,map->mnemonic,
					fs_mnemonic[F_VM_CNT],vm_cnt,
					fs_mnemonic[F_EXP_CNT],exp_cnt,
					fs_mnemonic[F_LGKM_CNT],lgkm_cnt);

	}else{
		/*manage the branch instructions*/
		if(S_BRANCH<=op&&op<=S_CBRANCH_EXECNZ) sopp_branch(c,i,map);
		else{/*the remaining sopp instructions*/
			s16 simm16;

			simm16=(s16)(i&0x0000ffff);
			src_out(c,"        0x%08x         %s %s=%u\n",
						i,map->mnemonic,
						fs_mnemonic[F_SIMM16],simm16);
		}
	}
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 sopc(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:sopc unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

#define SCC_STR_BUF_SZ sizeof("literal_constant")
static void scc_str(u8 *d,u16 v)
{
	d[0]=0;
	if(v<=103) snprintf(d,SCC_STR_BUF_SZ,"s%u",v);
	else if(v==106) strncpy(d,"vcc_lo",SCC_STR_BUF_SZ);
	else if(v==107) strncpy(d,"vcc_hi",SCC_STR_BUF_SZ);
	else if(v==108) strncpy(d,"tba_lo",SCC_STR_BUF_SZ);
	else if(v==109) strncpy(d,"tba_hi",SCC_STR_BUF_SZ);
	else if(v==110) strncpy(d,"tma_lo",SCC_STR_BUF_SZ);
	else if(v==111) strncpy(d,"tma_hi",SCC_STR_BUF_SZ);
	else if(112<=v&&v<=123) snprintf(d,SCC_STR_BUF_SZ,"ttmp%u",v-112);
	else if(v==124) strncpy(d,"m0",SCC_STR_BUF_SZ);
	else if(v==126) strncpy(d,"exec_lo",SCC_STR_BUF_SZ);
	else if(v==127) strncpy(d,"exec_hi",SCC_STR_BUF_SZ);
	else if(129<=v&&v<=192) snprintf(d,SCC_STR_BUF_SZ,"%u",v-129+1);
	else if(193<=v&&v<=208) snprintf(d,SCC_STR_BUF_SZ,"-%u",v-193+1);
	else if(v==240) strncpy(d,"0.5",SCC_STR_BUF_SZ);
	else if(v==241) strncpy(d,"-0.5",SCC_STR_BUF_SZ);
	else if(v==242) strncpy(d,"1.0",SCC_STR_BUF_SZ);
	else if(v==243) strncpy(d,"-1.0",SCC_STR_BUF_SZ);
	else if(v==244) strncpy(d,"2.0",SCC_STR_BUF_SZ);
	else if(v==245) strncpy(d,"-2.0",SCC_STR_BUF_SZ);
	else if(v==246) strncpy(d,"4.0",SCC_STR_BUF_SZ);
	else if(v==247) strncpy(d,"-4.0",SCC_STR_BUF_SZ);
	else if(v==251) strncpy(d,"vccz",SCC_STR_BUF_SZ);
	else if(v==252) strncpy(d,"execz",SCC_STR_BUF_SZ);
	else if(v==253) strncpy(d,"scc",SCC_STR_BUF_SZ);
	else if(v==254) strncpy(d,"lds_direct",SCC_STR_BUF_SZ);
	else if(v==255) strncpy(d,"literal_constant",SCC_STR_BUF_SZ);
	else if(256<=v&&v<=511) snprintf(d,SCC_STR_BUF_SZ,"v%u",v-256);
}

static s8 sop1(struct ctx *c)
{
	s8 r;
	u32 i;
	u8 ssrc0;
	u8 ssrc0_str[SCC_STR_BUF_SZ];
	u8 op;
	u8 sdst;
	u8 sdst_str[SCC_STR_BUF_SZ];
	struct i_mnemonic_map *map;
	struct il *il;

	i=le322cpu(*(u32*)(c->i));

	ssrc0=i&0xff;
	op=(i>>8)&0xff;
	sdst=(i>>16)&0x7f;

	scc_str(&ssrc0_str[0],(u16)ssrc0); 
	scc_str(&sdst_str[0],(u16)sdst);

	map=i_mnemonic_map_find(c,FMT_SOP1,(s16)op,"sop1");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	if(ssrc0==255){/*we have a literal constant*/
		u32 literal_constant;
		literal_constant=le322cpu(*((u32*)(c->i)+1));
		src_out(c,"        0x%08x         %s %s=%s %s=%s literal_constant=0x%08x\n",
					i,map->mnemonic,
					fs_mnemonic[F_SSRC0],&ssrc0_str[0],
					fs_mnemonic[F_SDST],&sdst_str[0],
					literal_constant);
	}else{
		src_out(c,"        0x%08x         %s %s=%s %s=%s\n",
					i,map->mnemonic,
					fs_mnemonic[F_SSRC0],&ssrc0_str[0],
					fs_mnemonic[F_SDST],&sdst_str[0]);
	}

	label_pending_insert(c,il);
	r=sizeof(i);
	if(ssrc0==255) r+=sizeof(u32);/*don't forget the literal constant*/
	return r;
}

static s8 vopc(struct ctx *c)
{
	struct i_mnemonic_map *map;
	u8 src0_str[SCC_STR_BUF_SZ];
	u32 i;
	u16 src0;
	u8 op;
	u8 vsrc1;
	struct il *il;

	i=le322cpu(*(u32*)(c->i));
	src0=i&0x1ff;
	op=(i>>17)&0xff;
	vsrc1=(i>>9)&0xff;

	scc_str(&src0_str[0],src0); 

	map=i_mnemonic_map_find(c,FMT_VOPC,(s16)op,"vopc");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%08x         %s %s=%s %s=v%u\n",
					i,map->mnemonic,
					fs_mnemonic[F_SRC0],&src0_str[0],
					fs_mnemonic[F_VSRC1],vsrc1);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 vop1(struct ctx *c)
{
	struct i_mnemonic_map *map;
	u8 src0_str[SCC_STR_BUF_SZ];
	u32 i;
	u16 src0;
	u8 op;
	u8 vdst;
	struct il *il;

	i=le322cpu(*(u32*)(c->i));
	src0=i&0x1ff;
	op=(i>>9)&0xff;
	vdst=(i>>17)&0xff;

	scc_str(&src0_str[0],src0); 
	map=i_mnemonic_map_find(c,FMT_VOP1,(s16)op,"vop1");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%08x          %s %s=%s %s=v%u\n",
						i,map->mnemonic,
						fs_mnemonic[F_SRC0],src0_str,
						fs_mnemonic[F_VDST],vdst);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 vop3b(struct ctx *c,u64 i,u16 op)
{
	u8 vdst;
	u8 sdst;
	u8 sdst_str[SCC_STR_BUF_SZ];
	u16 src0;
	u8 src0_str[SCC_STR_BUF_SZ];
	u16 src1;
	u8 src1_str[SCC_STR_BUF_SZ];
	u16 src2;
	u8 src2_str[SCC_STR_BUF_SZ];
	u8 omod;
	u8 neg;
	struct i_mnemonic_map *map;
	struct il *il;

	vdst=i&0xff;
	sdst=(i>>8)&0x7f;
	src0=(i>>32)&0x1ff;
	src1=(i>>41)&0x1ff;
	src2=(i>>50)&0x1ff;
	omod=(i>>59)&0x3;
	neg=(i>>61)&0x3;

	scc_str(&sdst_str[0],(u16)sdst); 
	scc_str(&src0_str[0],(u16)src0); 
	scc_str(&src1_str[0],(u16)src1);
	scc_str(&src2_str[0],(u16)src2);

	map=i_mnemonic_map_find(c,FMT_VOP3B,(s16)op,"vop3b");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%016x %s %s=v%u %s=%s %s=%s %s=%s %s=%s %s=%u %s=src%u\n",
					i,map->mnemonic,
					fs_mnemonic[F_VDST],vdst,
					fs_mnemonic[F_SDST],&sdst_str[0],
					fs_mnemonic[F_SRC0],&src0_str[0],
					fs_mnemonic[F_SRC1],&src1_str[0],
					fs_mnemonic[F_SRC2],&src2_str[0],
					fs_mnemonic[F_OMOD],omod,
					fs_mnemonic[F_NEG],neg);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 vop3a(struct ctx *c,u64 i,u16 op)
{
	u8 vdst;
	u8 vdst_str[SCC_STR_BUF_SZ];
	u8 abs;
	u8 clamp;
	u16 src0;
	u8 src0_str[SCC_STR_BUF_SZ];
	u16 src1;
	u8 src1_str[SCC_STR_BUF_SZ];
	u16 src2;
	u8 src2_str[SCC_STR_BUF_SZ];
	u8 omod;
	u8 neg;
	struct i_mnemonic_map *map;
	struct il *il;

	vdst=i&0xff;
	abs=(i>>8)&0x7;
	clamp=(i>>11)&0x1;
	src0=(i>>32)&0x1ff;
	src1=(i>>41)&0x1ff;
	src2=(i>>50)&0x1ff;
	omod=(i>>59)&0x3;
	neg=(i>>61)&0x3;

	scc_str(&src0_str[0],(u16)src0); 
	scc_str(&src1_str[0],(u16)src1);
	scc_str(&src2_str[0],(u16)src2);

	map=i_mnemonic_map_find(c,FMT_VOP3A,(s16)op,"vop3a");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	if(op<=255){/*in cmp ops, vdst is a sgprs or vcc*/
		scc_str(&vdst_str[0],(u16)vdst);
		src_out(c,"        0x%016x %s %s=%s %s=src%u %s=%u %s=%s %s=%s %s=%s %s=%u %s=src%u\n",
					i,map->mnemonic,
					fs_mnemonic[F_VDST],&vdst_str[0],
					fs_mnemonic[F_ABS],abs,
					fs_mnemonic[F_CLAMP],clamp,
					fs_mnemonic[F_SRC0],&src0_str[0],
					fs_mnemonic[F_SRC1],&src1_str[0],
					fs_mnemonic[F_SRC2],&src2_str[0],
					fs_mnemonic[F_OMOD],omod,
					fs_mnemonic[F_NEG],neg);
	}else src_out(c,"        0x%016x %s %s=v%u %s=src%u %s=%u %s=%s %s=%s %s=%u %s=src%u\n",
					i,map->mnemonic,
					fs_mnemonic[F_VDST],vdst,
					fs_mnemonic[F_ABS],abs,
					fs_mnemonic[F_CLAMP],clamp,
					fs_mnemonic[F_SRC0],&src0_str[0],
					fs_mnemonic[F_SRC1],&src1_str[0],
					fs_mnemonic[F_SRC2],&src2_str[0],
					fs_mnemonic[F_OMOD],omod,
					fs_mnemonic[F_NEG],neg);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 vop3(struct ctx *c)
{
	u64 i;
	u16 op;

	i=le642cpu(*(u64*)(c->i));

	op=(i>>17)&0x1ff;

	if(vop3_is_vop3b(op)) return vop3b(c,i,op);
	return vop3a(c,i,op);
}

static s8 vintrp(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:vintrp unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 ds(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:ds unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 mubuf(struct ctx *c)
{
	struct i_mnemonic_map *map;
	u64 i;
	u16 offset;
	u8 offen;
	u8 idxen;
	u8 glc;
	u8 addr64;
	u8 lds;
	u8 op;
	u8 vaddr;
	u8 vdata;
	u8 srsrc;
	u8 slc;
	u8 tfe;
	struct il* il;

	i=le642cpu(*(u64*)(c->i));
	offset=i&0xfff;
	offen=(i>>12)&1;
	idxen=(i>>13)&1;
	glc=(i>>14)&1;
	addr64=(i>>15)&1;
	lds=(i>>16)&1;
	op=(i>>18)&0xef;
	vaddr=(i>>32)&0xff;
	vdata=(i>>40)&0xff;
	srsrc=(i>>48)&0x1f;
	slc=(i>>54)&1;
	tfe=(i>>55)&1;

	map=i_mnemonic_map_find(c,FMT_MUBUF,(s16)op,"mubuf");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%016x %s %s=%u %s=%u %s=%u %s=%u %s=%u %s=%u %s=v%u %s=v%u %s=s%u %s=%u %s=%u\n",
						i,map->mnemonic,
						fs_mnemonic[F_OFFSET],offset,
						fs_mnemonic[F_OFFEN],offen,
						fs_mnemonic[F_IDXEN],idxen,
						fs_mnemonic[F_GLC],glc,
						fs_mnemonic[F_ADDR64],addr64,
						fs_mnemonic[F_LDS],lds,
						fs_mnemonic[F_VADDR],vaddr,
						fs_mnemonic[F_VDATA],vdata,
						fs_mnemonic[F_SRSRC],srsrc<<2,
						fs_mnemonic[F_SLC],slc,
						fs_mnemonic[F_TFE],tfe);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 mtbuf(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:mtbuf unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 mimg(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:mimg unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 export(struct ctx *c)
{
	u8 compr;
	u8 done;
	u8 vm;
	u8 vsrc0;
	u8 vsrc1;
	u8 vsrc2;
	u8 vsrc3;
	struct i_mnemonic_map *map;
	u8 tgt_str[sizeof("paramXX")];
	u64 i;
	u8 en;
	u8 tgt;
	struct il *il;

	i=le642cpu(*(u64*)(c->i));
	en=i&0xf;
	tgt=(i>>4)&0x3f;

	if(tgt<=7)
		snprintf(&tgt_str[0],sizeof("paramXX"),"mrt%u",tgt);
	else if(tgt==8)
		strncpy(&tgt_str[0],"mtrz",sizeof("paramXX"));
	else if(tgt==9)
		strncpy(&tgt_str[0],"null",sizeof("paramXX"));
	else if(12<=tgt&&tgt<=15)
		snprintf(tgt_str,sizeof("paramXX"),"pos%u",tgt-12);
	else if(32<=tgt&&tgt<=63)
		snprintf(&tgt_str[0],sizeof("paramXX"),"param%u",tgt-32);
    
	compr=(i>>10)&1;
	done=(i>>11)&1;
	vm=(i>>12)&1;
	vsrc0=(i>>32)&0xff;
	vsrc1=(i>>40)&0xff;
	vsrc2=(i>>48)&0xff;
	vsrc3=(i>>56)&0xff;

	map=i_mnemonic_map_find(c,FMT_EXP,0,"exp");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%016x %s %s=0x%x %s=%s %s=%u %s=%u %s=%u %s=v%u %s=v%u %s=v%u %s=v%u\n",
						i,map->mnemonic,
						fs_mnemonic[F_EN],en,
						fs_mnemonic[F_TGT],tgt_str,
						fs_mnemonic[F_COMPR],compr,
						fs_mnemonic[F_DONE],done,
						fs_mnemonic[F_VM],vm,
						fs_mnemonic[F_VSRC0],vsrc0,
						fs_mnemonic[F_VSRC1],vsrc1,
						fs_mnemonic[F_VSRC2],vsrc2,
						fs_mnemonic[F_VSRC3],vsrc3);
	label_pending_insert(c,il);
	return sizeof(i);
}

static s8 smrd(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:smrd unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 sopk(struct ctx *c)
{
	msg(c->msgs,"error:0x%lx:sopk unimplemented\n",c->i-c->m);
	return CMINGCNDIS_ERR;
}

static s8 sop2(struct ctx *c)
{
	s8 r;
	u32 i;
	u8 ssrc0;
	u8 ssrc0_str[SCC_STR_BUF_SZ];
	u8 ssrc1;
	u8 ssrc1_str[SCC_STR_BUF_SZ];
	u8 sdst;
	u8 sdst_str[SCC_STR_BUF_SZ];
	u8 op;
	struct i_mnemonic_map *map;
	u8 literal_offset;
	struct il *il;

	i=le322cpu(*(u32*)(c->i));

	ssrc0=i&0xff;
	ssrc1=(i>>8)&0xff;
	sdst=(i>>16)&0x7f;
	op=(i>>23)&0x7f;

	scc_str(&ssrc0_str[0],(u16)ssrc0); 
	scc_str(&ssrc1_str[0],(u16)ssrc1); 
	scc_str(&sdst_str[0],(u16)sdst); 

	map=i_mnemonic_map_find(c,FMT_SOP2,(s16)op,"sop2");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%08x         %s %s=%s %s=%s %s=%s",
					i,map->mnemonic,
					fs_mnemonic[F_SSRC0],&ssrc0_str[0],
					fs_mnemonic[F_SSRC1],&ssrc1_str[0],
					fs_mnemonic[F_SDST],&sdst_str[0]);
	literal_offset=1;
	if(ssrc0==255){
		u32 literal_constant;

		literal_constant=le322cpu(*((u32*)(c->i)+literal_offset));
		++literal_offset;
		src_out(c," ssrc0_literal_constant=0x%08x",literal_constant);
	}
	if(ssrc1==255){
		u32 literal_constant;

		literal_constant=le322cpu(*((u32*)(c->i)+literal_offset));
		src_out(c," ssrc1_literal_constant=0x%08x",literal_constant);
	}
	src_out(c,"\n");
	
	label_pending_insert(c,il);
	r=sizeof(i);
	if(ssrc0==255) r+=sizeof(u32);
	if(ssrc1==255) r+=sizeof(u32);
	return r;
}

static s8 vop2(struct ctx *c)
{
	u8 src0_str[SCC_STR_BUF_SZ];
	struct i_mnemonic_map *map;
	u32 i;
	u16 src0;
	u8 vsrc1;
	u8 vdst;
	u8 op;
	struct il *il;

	i=le322cpu(*(u32*)(c->i));
	src0=i&0x1ff;
	vsrc1=(i>>0)&0xff;
	vdst=(i>>17)&0xff;
	op=(i>>25)&0x3f;

	scc_str(&src0_str[0],src0); 

	map=i_mnemonic_map_find(c,FMT_VOP2,(s16)op,"vop2");
	if(!map) return CMINGCNDIS_ERR;

	il=ils_mark(c,c->i,*c->src);

	src_out(c,"        0x%016x %s %s=%s %s=v%u %s=v%u\n",
						i,map->mnemonic,
						fs_mnemonic[F_SRC0],src0_str,
						fs_mnemonic[F_VSRC1],vsrc1,
						fs_mnemonic[F_VDST],vdst);
	label_pending_insert(c,il);
	return sizeof(i);
}

/*return how long the machine instruction was*/
static s8 i_dis(struct ctx *c)
{
	s8 r;
	u32 i0;

	i0=le322cpu(*(u32*)(c->i));
 
		if((i0&0xff800000)==0xbf800000) r=sopp(c);
	else	if((i0&0xff800000)==0xbf000000) r=sopc(c);
	else	if((i0&0xff800000)==0xbe800000) r=sop1(c);
	else	if((i0&0xfe000000)==0x7c000000) r=vopc(c);
	else	if((i0&0xfe000000)==0x7e000000) r=vop1(c);
	else	if((i0&0xfc000000)==0xd0000000) r=vop3(c);
	else	if((i0&0xfc000000)==0xa8000000) r=vintrp(c);
	else	if((i0&0xfc000000)==0xd8000000) r=ds(c);
	else	if((i0&0xfc000000)==0xe0000000) r=mubuf(c);
	else	if((i0&0xfc000000)==0xe8000000) r=mtbuf(c);
	else	if((i0&0xfc000000)==0xf0000000) r=mimg(c);
	else	if((i0&0xfc000000)==0xf8000000) r=export(c);
	else	if((i0&0xf8000000)==0xc0000000) r=smrd(c);
	else	if((i0&0xf0000000)==0xb0000000) r=sopk(c);
	else	if((i0&0xc0000000)==0x80000000) r=sop2(c);
	else	if((i0&0x80000000)==0x00000000) r=vop2(c);
	else{
		msg(c->msgs,"error:0x%lx:instruction unknown\n",c->i-c->m);
		r=CMINGCNDIS_ERR;
	}
	return r;
}

/*we don't want to rely on the elf loader for that*/
void cmingcndis_static_init(void)
{
	i_mnemonic_maps_init();
	fs_mnemonic_init();
}

s8 cmingcndis_dis(	u8 *m,
			u64 m_sz,
			u8 **src,
			u64 *src_sz,
			u8 **msgs,
			u64 *msgs_sz)
{
	struct msgs_ctx msgs_c;
	struct ctx c;
	u8 *m_e;
	s8 r;

	r=0;

	if(msgs&&msgs_sz){
		*msgs=0;
		*msgs_sz=0;
	}

	msgs_c.msgs=msgs;
	msgs_c.sz=msgs_sz;

	if(!m||m_sz==0){
		msg(&msgs_c,"machine code empty(p=0x%p:sz=%d)\n",m,m_sz);
		return CMINGCNDIS_ERR;
	}

	c.m=m;
	c.src=src;
	*c.src=0;
	c.src_sz=src_sz;
	*c.src_sz=0;
	
	c.ils=0;
	c.ils_n=0;
	c.next_label=0;
	c.labels_pending=0;
	c.labels_pending_n=0;
	
	c.msgs=&msgs_c;

	/*start work here*/
	c.i=m;
	m_e=m+m_sz;/*points right after the last byte*/
	loop{
		if(c.i>=m_e) break;

		r=i_dis(&c);
		if(r<0) goto err_unmap_src;
		c.i+=r;
	}
	goto unmap_ils;

err_unmap_src:
	src_unmap(&c);

unmap_ils:
	ils_unmap(&c);
	labels_pending_unmap(&c);
	return r;
}
