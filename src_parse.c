/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
/*------------------------------------------------------------------------------
compiler stuff
------------------------------------------------------------------------------*/
#include <stdarg.h>
/*------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
ulinux stuff
------------------------------------------------------------------------------*/
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/block/conv/decimal/decimal.h>
#include <ulinux/utils/ascii/block/conv/hexadecimal/hexadecimal.h>
#include <ulinux/utils/ascii/block/conv/binary/binary.h>

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
#define SRC_PARSE_C
#include "libcmingcnasm-private.h"
#undef SRC_PARSE_C
/*----------------------------------------------------------------------------*/

#define LAST_LINE_PROCESSING 0x01
#define HAVE_LABEL           0x02
struct ctx{
	u8 *src;
	u64 src_sz;

	/*source line context*/
	u64 src_l;
	u8 src_pathname[SRC_PATHNAME_SZ_MAX+1];

	struct i *is;
	u64 is_n;

	u8 flgs;

	/*line context*/
	u64 l;		/*source file line number*/
	u8 *p;		/*up to p of the line has been processed*/
	u8 *kw_e;	/*points on the last char of the current keyword*/
	u8 *l_s;
	u8 *l_e;	/*point to line '/n' or right after the end of file*/

	/*instruction context*/
	s8 (*f_parser)(struct ctx *c);

	struct msgs_ctx *msgs;
};

/*this is a brutal slab, remapping each time we grow*/
static void i_new(struct ctx *c)
{
	sl addr;
	struct i* i;

	if(c->is_n==0){/*first allocation, then mmapping*/
		addr=mmap(sizeof(*c->is),PROT_READ|PROT_WRITE,MAP_PRIVATE
							|MAP_ANONYMOUS,-1);
	}else{/*grow mapping*/
		u64 old_len;
		u64 new_len;

		old_len=c->is_n*sizeof(*c->is);
		new_len=old_len+sizeof(*c->is);

		addr=mremap(c->is,old_len,new_len,MREMAP_MAYMOVE);
	}

	c->is=(struct i*)addr;
	i=&c->is[c->is_n];
	++c->is_n;

	i->fs[0].f=F_INVALID;
	i->l=c->l;

	i->src_l=c->src_l;
	i->src_pathname[SRC_PATHNAME_SZ_MAX]=0;
	strncpy(&i->src_pathname[0],&c->src_pathname[0],SRC_PATHNAME_SZ_MAX);
}

static u8 is_blank(u8 *p)
{
	u8 r;

	r=0;

	if(*p==' '||*p=='\t') r=1;
	return r;
}

static void l_next(struct ctx *c)
{
	c->p=c->l_e+1;
}

static u8 is_l_e(struct ctx *c)
{
	if(c->p==c->l_e) return 1;
	return 0;
}

static void l_blanks_skip(struct ctx *c)
{
	loop{
		if(c->p>=c->l_e) break;
    		if(!is_blank(c->p)) break;
		++c->p;
	}
}

/*c->p char content must not be blank*/
static void l_kws_next(struct ctx *c)
{
	c->kw_e=c->p;
	loop{
		if(c->kw_e+1>=c->l_e) break;
		if(is_blank(c->kw_e+1)) break;
		++c->kw_e;
	}
}

static void l_kw_consumed(struct ctx *c)
{
	c->p=c->kw_e+1;
}

#define MSG(x,...) msg(c->msgs,"error:source parse:%s:%u:pp(%u:%u):" x,\
c->src_pathname,c->src_l,c->l,c->p-c->l_s+1,##__VA_ARGS__)
static s8 l_kw_label(struct ctx *c)
{
	struct i *i;

	i=&c->is[c->is_n-1];
	if(c->flgs&HAVE_LABEL){
		i=&c->is[c->is_n-1];
		MSG("already have a label on line %u pp(%u) in %s\n",i->src_l,
							i->l,i->src_pathname);
		return CMINGCNASM_ERR;
	}
	i_new(c);/*ready an instruction with the label*/

	i=&c->is[c->is_n-1];
	i->label_s=c->p;
	i->label_e=c->kw_e-1;
	return 0;
}

static s8 l_kw_i(struct ctx *c)
{
	struct i *i;
	u64 i_sz;

	/*a label may have already instanciated an instruction*/
	if(!(c->flgs&HAVE_LABEL)) i_new(c);

	i=&c->is[c->is_n-1];

	i->map=&i_mnemonic_maps[0];
	i_sz=c->kw_e-c->p+1;

	loop{
		if(i->map->mnemonic==0) break;
		if(strncmp(i->map->mnemonic,c->p,i_sz)==0) break;
		++i->map;
	}

	if(i->map->mnemonic==0){
		MSG("unknown instruction\n");
		return CMINGCNASM_ERR;
	}
	return 0;
}
#undef MSG

#define MSG(x) msg(c->msgs,"error:source parse:%s:%u:pp(%u:%u):%s" \
" field" x,c->src_pathname,c->src_l,c->l,val-c->l_s+1,fs_mnemonic[f])
static s8 f_scc(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	s8 r0;
	u64 val_sz;

	r0=CMINGCNASM_ERR;/*in error state by default*/
	val_sz=c->kw_e-val+1;

	if(val_sz>2&&val[0]=='s'){
		u8 sgpr_idx;
		u8 r1;

		r1=dec2u8(&sgpr_idx,val+1,c->kw_e);

		if(r1){
			if(sgpr_idx>103){
				MSG("sgpr index above 103\n");
				goto exit;
			}
			i_f->val=(u16)sgpr_idx;
			r0=0;
			goto exit;
		}
	}
	if(!strncmp(val,"vcc_lo",val_sz)){
		i_f->val=106;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"vcc_hi",val_sz)){
		i_f->val=107;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"tba_lo",val_sz)){
		i_f->val=108;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"tba_hi",val_sz)){
		i_f->val=109;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"tma_lo",val_sz)){
		i_f->val=110;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"tma_hi",val_sz)){
		i_f->val=111;
		r0=0;
		goto exit;
	}
	if(val_sz>=cs_n("ttmp")+1){
		if(!strncmp(val,"ttmp",cs_n("ttmp"))){
			u8 ttmp_idx;
			u8 r1;

			r1=dec2u8(&ttmp_idx,val+cs_n("ttmp"),c->kw_e);

			if(r1){
				if(ttmp_idx>11){
					MSG("ttmp index above 11\n");
					goto exit;
				}
				i_f->val=(u16)ttmp_idx+112;
				r0=0;
				goto exit;
			}
			MSG("ttmp index is not decimal\n");
			goto exit;
		}
  	}
	if(!strncmp(val,"m0",val_sz)){
		i_f->val=124;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"exec_lo",val_sz)){
		i_f->val=126;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"exec_hi",val_sz)){
		i_f->val=127;
		r0=0;
		goto exit;
	}

	if(val_sz==1||val_sz==2){
		u8 integer;
		u8 r1;

		r1=dec2u8(&integer,val,c->kw_e);

		if(r1){
			if(integer>64){
				MSG("integer above 64\n");
				goto exit;
			}
			i_f->val=(u16)integer+128;
			r0=0;
			goto exit;
		}
	}
	if(val[0]=='-'&&(val_sz==2||val_sz==3)){
		u8 ninteger;
		u8 r1;

		r1=dec2u8(&ninteger,val+1,c->kw_e);

		if(r1){
			if(ninteger>64||ninteger==0){
				MSG("negative integer below 64 or is 0\n");
				goto exit;
			}
			i_f->val=(u16)ninteger+193;
			r0=0;
			goto exit;
		}
	}
	if(!strncmp(val,"0.5",val_sz)){
		i_f->val=240;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"-0.5",val_sz)){
		i_f->val=241;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"1.0",val_sz)){
		i_f->val=242;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"-1.0",val_sz)){
		i_f->val=243;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"2.0",val_sz)){
		i_f->val=244;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"-2.0",val_sz)){
		i_f->val=245;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"4.0",val_sz)){
		i_f->val=246;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"-4.0",val_sz)){
		i_f->val=247;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"vccz",val_sz)){
		i_f->val=251;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"execz",val_sz)){
		i_f->val=252;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"scc",val_sz)){
		i_f->val=253;
		r0=0;
		goto exit;
	}
	if(!strncmp(val,"literal_constant",val_sz)){
		i_f->val=255;
		r0=0;
		goto exit;
	}
	if(val_sz>=2&&val[0]=='v'){
		u8 vgpr_idx;
		u8 r1;

		r1=dec2u8(&vgpr_idx,val+1,c->kw_e);

		if(r1){
			i_f->val=(u16)vgpr_idx+256;
			r0=0;
			goto exit;
		}
  	}
 
  	MSG("has an unknown scc value\n");
exit:
	return r0; 
}

static s8 f_bool(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	u8 val_sz=c->kw_e-val+1;
	s8 r0;

	r0=0;

	if(val_sz!=1){r0=CMINGCNASM_ERR;goto exit;}

	if(val[0]=='0') i_f->val=0;
	else if(val[0]=='1') i_f->val=1;
	else r0=CMINGCNASM_ERR;

exit:
	if(r0!=0) MSG("has an invalid boolean value\n");
	return r0;
}

static s8 f_vgpr(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	u8 vgpr_idx;
  	u64 val_sz;
	u8 r;

  	val_sz=c->kw_e-val+1;
	r=dec2u8(&vgpr_idx,val+1,c->kw_e);

	if(val_sz<2||val[0]!='v'){
		MSG("has an unknown vgpr value\n");
		return CMINGCNASM_ERR;
	}

	if(!r){
		MSG("has an invalid vgpr index\n");
		return CMINGCNASM_ERR;
	}

	i_f->val=(u16)vgpr_idx;
	return 0;
}

static s8 f_sgpr_mod_4(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	u8 sgpr_idx;
	u8 r;
	u64 val_sz;

	val_sz=c->kw_e-val+1;

	if(val_sz<2&&val[0]!='s'){
		MSG("has an unknown sgpr value\n");
		return CMINGCNASM_ERR;
	}

	r=dec2u8(&sgpr_idx,val+1,c->kw_e);
	if(!r){
		MSG("has an invalid sgpr index\n");
		return CMINGCNASM_ERR;
	}

	if(sgpr_idx>103){
		MSG("sgpr index above 103\n");
		return CMINGCNASM_ERR;
	}

	if(sgpr_idx%4){
		MSG("sgpr index in not 4 register aligned\n");
		return CMINGCNASM_ERR;
	}
  
	i_f->val=(u16)(sgpr_idx>>2);
	return 0;
}

static s8 f_u16(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	u8 r;
	u64 val_sz;

	val_sz=c->kw_e-val+1;

	if(val_sz>=3){
		if(val[0]=='0'&&val[1]=='b'){/*binary value*/
			r=bin2u16(&i_f->val,&val[2],c->kw_e);
			if(!r){
				MSG("has not a valid binary value\n");
				return CMINGCNASM_ERR;
			}
			return 0;
		}

		if(val[0]=='0'&&val[1]=='x'){/*hexadecimal value*/
			r=hex2u16(&i_f->val,&val[2],c->kw_e);
			if(!r){
				MSG("has not a valid hexadecimal value\n");
				return CMINGCNASM_ERR;
			}
			return 0;
		}
	}

	/*decimal*/
	r=dec2u16(&i_f->val,val,c->kw_e);
	if(!r){
		MSG("has not a valid decimal value\n");
		return CMINGCNASM_ERR;
	}
	return 0;
}

static s8 f_tgt(struct ctx *c,u8 f,struct i_f *i_f,u8 *val)
{
	u64 val_sz;

	val_sz=c->kw_e-val+1;

	if(val_sz==cs_n("mrt")+1){
		if(!strncmp(val,"mrt",cs_n("mrt"))){
			u8 mrt_idx;
			u8 r;

			r=dec2u8(&mrt_idx,val+cs_n("mrt"),c->kw_e);

			if(r){
				if(mrt_idx>=8){
					MSG("mrt index above 7\n");
					return CMINGCNASM_ERR;
				}
				i_f->val=(u16)mrt_idx;
				return 0;
			}
			MSG("mrt index is not decimal\n");
			return CMINGCNASM_ERR;
		}
  	}
	if(!strncmp(val,"mrtz",val_sz)){
		i_f->val=8;
		return 0;
	}
	if(!strncmp(val,"null",val_sz)){
		i_f->val=9;
		return 0;
	}
	if(val_sz==cs_n("pos")+1){
		if(!strncmp(val,"pos",cs_n("pos"))){
			u8 pos_idx;
			u8 r;

			r=dec2u8(&pos_idx,val+cs_n("pos"),c->kw_e);

			if(r){
				if(pos_idx>=4){
					MSG("pos index above 3\n");
					return CMINGCNASM_ERR;
				}
				i_f->val=(u16)pos_idx+12;
				return 0;
			}
			MSG("pos index is not decimal\n");
			return CMINGCNASM_ERR;
		}
  	}
	if(val_sz>=cs_n("param")+1){
		if(!strncmp(val,"param",cs_n("param"))){
			u8 param_idx;
			u8 r;

			r=dec2u8(&param_idx,val+cs_n("param"),c->kw_e);

			if(r){
				if(param_idx>=32){
					MSG("param index above 31\n");
					return CMINGCNASM_ERR;
				}
				i_f->val=(u16)param_idx+32;
				return 0;
			}
			MSG("param index is not decimal\n");
			return CMINGCNASM_ERR;
		}
	}

	MSG("has an unknown value\n");
	return CMINGCNASM_ERR;
}
#undef MSG

static s8 (*fs_val_parser[F_INVALID])(struct ctx *c,u8 f,struct i_f *i_f,
								u8 *val);

void fs_val_parser_init(void)
{
fs_val_parser[F_SSRC0]=0;
fs_val_parser[F_SSRC1]=0;
fs_val_parser[F_SDST]=0;
fs_val_parser[F_SIMM16]=0;
fs_val_parser[F_OFFSET]=0;
fs_val_parser[F_IMM]=0;
fs_val_parser[F_SBASE]=0;
fs_val_parser[F_SRC0]=f_scc;
fs_val_parser[F_SRC1]=f_scc;
fs_val_parser[F_SRC2]=f_scc;
fs_val_parser[F_VSRC0]=f_vgpr;
fs_val_parser[F_VSRC1]=f_vgpr;
fs_val_parser[F_VSRC2]=f_vgpr;
fs_val_parser[F_VSRC3]=f_vgpr;
fs_val_parser[F_VDST]=f_vgpr;
fs_val_parser[F_ABS]=f_u16;
fs_val_parser[F_CLAMP]=f_bool;
fs_val_parser[F_OMOD]=f_u16;
fs_val_parser[F_NEG]=f_u16;
fs_val_parser[F_VSRC]=0;
fs_val_parser[F_ATTRCHAN]=0;
fs_val_parser[F_ATTR]=0;
fs_val_parser[F_OFFSET0]=0;
fs_val_parser[F_OFFSET1]=0;
fs_val_parser[F_GDS]=0;
fs_val_parser[F_ADDR]=0;
fs_val_parser[F_DATA0]=0;
fs_val_parser[F_DATA1]=0;
fs_val_parser[F_OFFEN]=f_bool;
fs_val_parser[F_IDXEN]=f_bool;
fs_val_parser[F_GLC]=0;
fs_val_parser[F_ADDR64]=0;
fs_val_parser[F_LDS]=0;
fs_val_parser[F_VADDR]=0;
fs_val_parser[F_VDATA]=f_vgpr;
fs_val_parser[F_SRSRC]=f_sgpr_mod_4;
fs_val_parser[F_SLC]=0;
fs_val_parser[F_TFE]=0;
fs_val_parser[F_SOFFSET]=f_scc;
fs_val_parser[F_DFMT]=0;
fs_val_parser[F_NFMT]=0;
fs_val_parser[F_DMASK]=0;
fs_val_parser[F_UNORM]=0;
fs_val_parser[F_DA]=0;
fs_val_parser[F_R128]=0;
fs_val_parser[F_LWE]=0;
fs_val_parser[F_SSAMP]=0;
fs_val_parser[F_EN]=f_u16;
fs_val_parser[F_TGT]=f_tgt;
fs_val_parser[F_COMPR]=f_bool;
fs_val_parser[F_DONE]=f_bool;
fs_val_parser[F_VM]=f_bool;
fs_val_parser[F_VM_CNT]=f_u16;
fs_val_parser[F_EXP_CNT]=f_u16;
fs_val_parser[F_LGKM_CNT]=f_u16;
};

#define MSG(x,...) msg(c->msgs,"error:source parse:%s:%u:pp(%u:%u):" x,\
c->src_pathname,c->src_l,c->l,c->p-c->l_s+1,##__VA_ARGS__)
static s8 l_kw_f(struct ctx *c)
{
	u8 i_f;
	u64 mnemonic_sz;
	u8 *mnemonic_e;
	u8 f;

	/*make room for a new field in the instruction*/
	i_f=0;
	loop{
		if(i_f>=FS_MAX) break; 
		if(c->is[c->is_n-1].fs[i_f].f==F_INVALID) break;
		++i_f;
	}
	if(i_f==FS_MAX){
		MSG("no more room for field in instruction\n");
		return CMINGCNASM_ERR;
	}
	if(i_f<(FS_MAX-1)) c->is[c->is_n-1].fs[i_f+1].f=F_INVALID;

	/*locate value separator '='*/
	mnemonic_e=c->p;
	loop{
		if(mnemonic_e>c->kw_e||*mnemonic_e=='=') break;
		++mnemonic_e;
	}
  	if(mnemonic_e>=c->kw_e){
		MSG("field value separator not found or missing value\n");
		return CMINGCNASM_ERR;
	}
	mnemonic_e--;

	/*find proper field index*/
	mnemonic_sz=mnemonic_e-c->p+1;
	f=0;
	loop{
		if(f>=F_INVALID) break;
    		if(!strncmp(c->p,fs_mnemonic[f],mnemonic_sz)) break;
		++f;
	}
  	if(f==F_INVALID){
		MSG("field mnemonic not found\n");
		return CMINGCNASM_ERR;
	}

	c->is[c->is_n-1].fs[i_f].f=f;

	/*parse value*/
	if(!fs_val_parser[f]){
		MSG("%s field hasn't a parser\n",fs_mnemonic[f]);
		return CMINGCNASM_ERR;
	}
	/*call the parser*/
	return fs_val_parser[f](c,f,&c->is[c->is_n-1].fs[i_f],mnemonic_e+1+1);
}
#undef MSG

static void l_next_init(struct ctx *c)
{
	++c->l;
	++c->src_l;
	c->l_s=c->p;
	c->l_e=c->p;

	loop{
		if(c->l_e>=c->src+c->src_sz||*c->l_e=='\n') break;
		++c->l_e;
	}
	if(c->l_e==c->src+c->src_sz) c->flgs|=LAST_LINE_PROCESSING;
}

#define MSG(x) msg(c->msgs,"error:source parse:pp(%u):line preprocessor" \
" directive:" x,c->l)
static s8 pp(struct ctx *c)
{
	u8 *p;
	u8 *pp_kw_e;
	u64 src_l;
	u8 r;
	u8 *d;
	u8 *d_e;

	/*skip white space(s) till the source file line number*/
	p=c->p+1;/*skip '#'*/
	loop{
		if(p>=c->l_e) break;
		if(*p!=' ') break;
		++p;
	}
	if(p==c->l_e){
		MSG("missing source file line number\n");
		return CMINGCNASM_ERR;
	}

	/*get the source file line number keyword*/
	pp_kw_e=p;
	loop{
		if(pp_kw_e>=c->l_e) break;
  		if(!is_digit(*pp_kw_e)) break;
  		++pp_kw_e;
	}
	if(pp_kw_e==c->l_e){
		MSG("missing source file pathname\n");
		return CMINGCNASM_ERR;
	}
	pp_kw_e--;
 
	/*convert the source file line number keyword */
	r=dec2u64(&src_l,p,pp_kw_e);
	if(!r){
		MSG("does not have a valide line number\n");
		return CMINGCNASM_ERR;
	}
	p=pp_kw_e+1;
	/*store the source line number, swallowing itself*/
	c->src_l=src_l-1;

	/*skip white space(s) till the source file pathname*/
	loop{
		if(p>=c->l_e) break;
    		if(*p!=' ') break;
    		++p;
	}
	/*the pathname must be enclosed within '"' chars*/
	if(p==c->l_e||*p!='"'){
		MSG("missing a source file pathname or it's missing the starting \"\n");
		return CMINGCNASM_ERR;
	}
	++p;/*skip '"'*/
 
	/*
	 *Get the source file pathname, delimitted with '"' char. Escaping is
	 *done inside the pathname.
	 */
	d=&c->src_pathname[0];
	/*right after the last char*/
	d_e=&c->src_pathname[0]+SRC_PATHNAME_SZ_MAX;
	loop{
		if(p>=c->l_e||*p=='"'||d>d_e) break;
		if(*p=='\\'){
      			if((p+1)==c->l_e||(d+1)==d_e) break;/*test ahead*/
			++p;
		}
		*d++=*p++; 
	}
	if(*p!='"'){
		MSG("unable to get source pathname\n");
		return CMINGCNASM_ERR;
	}
	*d=0;
	return 0;
}
#undef MSG

s8 src_parse(u8 *src,u64 src_sz,u8 *src_pathname_default,struct i **is,
						u64 *is_n,struct msgs_ctx *msgs)
{
	struct ctx c;
	s8 r;

	r=0;

	c.src=src;
	c.src_sz=src_sz;
	c.is=0;
	c.is_n=0;
	c.p=src;
	c.flgs=0;
	c.l=0;
	c.src_l=0;
	strncpy(&c.src_pathname[0],src_pathname_default,SRC_PATHNAME_SZ_MAX);
	c.src_pathname[SRC_PATHNAME_SZ_MAX]=0;
	c.msgs=msgs;

	loop{
		l_next_init(&c);

		if(*c.p=='#'){/*preprocessor line*/
			r=pp(&c);
			if(r!=0) goto exit;
			l_next(&c);
			continue;
		}

		/*skip any blanks at the beginning of the line*/
		l_blanks_skip(&c);
		if(is_l_e(&c)){/*this is a blank line*/
			l_next(&c);
			continue;
		}

		l_kws_next(&c);/*fetch first keyword*/

		/*------------------------------------------------------------*/
		/*label if any*/
		if(*c.kw_e==':'){/*first keyword is a label*/
			r=l_kw_label(&c);
			if(r!=0) goto exit;

			l_kw_consumed(&c);/*skip the processed label*/

			l_blanks_skip(&c);/*skip any blanks after the label*/

			if(is_l_e(&c)){/*label is standalone on its line*/
				c.flgs|=HAVE_LABEL;
				l_next(&c);
				continue;
			}

			l_kws_next(&c);/*fetch second keyword*/
		}
		/*------------------------------------------------------------*/

		/*------------------------------------------------------------*/
		/*instruction*/
		r=l_kw_i(&c);/*that keyword must be an instruction*/
		if(r!=0) goto exit;

		c.flgs&=~HAVE_LABEL;/*reset label presence*/

		l_kw_consumed(&c);/*skip the processed instruction*/

		l_blanks_skip(&c);/*try to reach the first instruction field*/
		/*------------------------------------------------------------*/

		/*------------------------------------------------------------*/
		/*processing loop of fields*/
		loop{
			if(is_l_e(&c)) break;

			l_kws_next(&c);/*fetch next keyword*/

			r=l_kw_f(&c);/*that keyword must be a field*/
			if(r!=0) goto exit;

			l_kw_consumed(&c);/*skip the processed field*/

			l_blanks_skip(&c);/*try to reach next field*/
		}
    		/*------------------------------------------------------------*/

		l_next(&c);
		if(c.flgs&LAST_LINE_PROCESSING) break;
	}

	*is=&c.is[0];
	*is_n=c.is_n;

exit:
	if(r!=0&&c.is_n) munmap(&c.is[0],sizeof(struct i)*c.is_n);
	return r;
}
