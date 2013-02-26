//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//------------------------------------------------------------------------------
//compiler stuff
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//ulinux stuff
//------------------------------------------------------------------------------
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/block/conv/decimal/decimal.h>
#include <ulinux/utils/ascii/block/conv/hexadecimal/hexadecimal.h>
#include <ulinux/utils/ascii/block/conv/binary/binary.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//public
//------------------------------------------------------------------------------
#include "cmingcnasm.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//private
//------------------------------------------------------------------------------
#include "msgs.h"
#include "is.h"
#include "libcmingcnasm-private.h"
//------------------------------------------------------------------------------

#define LAST_LINE_PROCESSING 0x01
#define HAVE_LABEL           0x02
struct ctx{
  k_u8 *src;
  k_s32 src_sz;

  //source line context
  k_s32 src_l;
  k_u8 src_pathname[SRC_PATHNAME_SZ_MAX+1];

  struct i *is;
  k_s32 is_last;

  k_u8 flgs;

  //line context
  k_s32 l;   //source file line number
  k_u8 *p;   //up to p of the line has been processed
  k_u8 *kw_e;//points on the last char of the current keyword
  k_u8 *l_s;
  k_u8 *l_e; //point to line '/n' or right after the end of file

  //instruction context
  k_s8 (*f_parser)(struct ctx *c);

  struct msgs_ctx *msgs;
};

#define MSG(x) msg(c->msgs,_("error:source parse:%s:%d:pp(%d):" x),\
c->src_pathname,c->src_l,c->l,addr)
//this is a brutal slab, remapping each time we grow
static k_s8 i_new(struct ctx *c)
{
  k_s8 r=0;
  k_l addr;

  if(c->is_last==-1){//first allocation, then mmapping
    addr=sysc(mmap,6,0,sizeof(*c->is),K_PROT_READ|K_PROT_WRITE,
                                            K_MAP_PRIVATE|K_MAP_ANONYMOUS,-1,0);
    if(K_ISERR(addr)){
      r=MSG("mmap(%ld):unable to mmap instruction slab\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
  }else{//grow mapping
    k_s32 old_len=(c->is_last+1)*sizeof(*c->is);
    k_s32 new_len=old_len+sizeof(*c->is);

    addr=sysc(mremap,5,c->is,old_len,new_len,K_MREMAP_MAYMOVE,0);
    if(K_ISERR(addr)){
      r=MSG("mremap(%ld):unable to remap instruction slab\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
  }

  c->is=(struct i*)addr;
  ++c->is_last;

  c->is[c->is_last].fs[0].f=F_INVALID;
  c->is[c->is_last].l=c->l;

  c->is[c->is_last].src_l=c->src_l;
  c->is[c->is_last].src_pathname[SRC_PATHNAME_SZ_MAX]=0;
  u_a_strncpy(&c->is[c->is_last].src_pathname[0],&c->src_pathname[0],
                                                           SRC_PATHNAME_SZ_MAX);
exit:
  return r; 
}
#undef MSG

k_s8 is_unmap(struct i *is,k_s32 is_last,struct msgs_ctx *msgs)
{
  if(is_last==-1) return 0;//nothing to unmap

  k_l r=sysc(munmap,2,is,sizeof(*is)*(is_last+1));
  if(K_ISERR(r)){
      r=(k_l)msg(msgs,_("munmap(%ld):unable to unmap instructions slab\n"),r);
      if(!r) r=CMINGCNASM_ERR;
  }
  return (k_s8)r;
}

static k_u8 is_blank(k_u8 *p)
{
  k_u8 r=0;
  if(*p==' '||*p=='\t') r=1;
  return r;
}

static void l_next(struct ctx *c)
{
  c->p=c->l_e+1;
}

static k_u8 is_l_e(struct ctx *c)
{
  if(c->p==c->l_e) return 1;
  return 0;
}

static void l_blanks_skip(struct ctx *c)
{
  while(c->p<c->l_e){
    if(!is_blank(c->p)) break;
    ++c->p;
  }
}

//c->p char content must not be blank
static void l_kws_next(struct ctx *c)
{
  c->kw_e=c->p;
  while(c->kw_e+1<c->l_e){
    if(is_blank(c->kw_e+1)) break;
    ++c->kw_e;
  }
}

static void l_kw_consumed(struct ctx *c)
{
  c->p=c->kw_e+1;
}

#define MSG(x,...) msg(c->msgs,_("error:source parse:%s:%d:pp(%d:%d):" x),\
c->src_pathname,c->src_l,c->l,c->p-c->l_s+1,##__VA_ARGS__)
static k_s8 l_kw_label(struct ctx *c)
{
  k_s8 r;
  if(c->flgs&HAVE_LABEL){
    r=MSG("already have a label on line %d pp(%d) in %s\n",
                                    c->is[c->is_last].src_l,c->is[c->is_last].l,
                                                c->is[c->is_last].src_pathname);
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
  r=i_new(c);//ready an instruction with the label
  if(r!=0) goto exit;

  struct i *i=&c->is[c->is_last];
  i->label_s=c->p;
  i->label_e=c->kw_e-1;

exit:
  return r;
}

static k_s8 l_kw_i(struct ctx *c)
{
  k_s8 r=0;
  //a label may have already instanciated an instruction
  if(!(c->flgs&HAVE_LABEL)){
    r=i_new(c);
    if(r!=0) goto exit;
  }

  struct i *i=&c->is[c->is_last];

  i->map=&i_mnemonic_maps[0];
  k_ul i_sz=c->kw_e-c->p+1;

  while(i->map->mnemonic!=0){
    if(u_a_strncmp(i->map->mnemonic,c->p,i_sz)==0) break;
    ++i->map;
  }
  if(i->map->mnemonic==0){
    r=MSG("unknown instruction\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
exit:
  return r;
}
#undef MSG

#define MSG(x) msg(c->msgs,_("error:source parse:%s:%d:pp(%d:%d):%s field " \
x),c->src_pathname,c->src_l,c->l,val-c->l_s+1,fs_mnemonic[f])
static k_s8 f_scc(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s8 r0=CMINGCNASM_ERR;//in error state by default

  k_ul val_sz=c->kw_e-val+1;

  if(val_sz>2&&val[0]=='s'){
    k_u8 sgpr_idx;
    k_ut r1=u_a_strict_dec2u8_blk(&sgpr_idx,val+1,c->kw_e);
    if(r1){
      if(sgpr_idx>103){
        k_s8 r2=MSG("sgpr index above 103\n");
        if(r2) r0=r2;
        goto exit;
      }
      i_f->val=(k_u16)sgpr_idx;
      r0=0;
      goto exit;
    }
  }
  if(!u_a_strncmp(val,_("vcc_lo"),val_sz)){
    i_f->val=106;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("vcc_hi"),val_sz)){
    i_f->val=107;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("tba_lo"),val_sz)){
    i_f->val=108;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("tba_hi"),val_sz)){
    i_f->val=109;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("tma_lo"),val_sz)){
    i_f->val=110;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("tma_hi"),val_sz)){
    i_f->val=111;
    r0=0;goto exit;
  }
  if(val_sz>=cs_n("ttmp")+1){
    if(!u_a_strncmp(val,_("ttmp"),cs_n("ttmp"))){
      k_u8 ttmp_idx;
      k_ut r1=u_a_strict_dec2u8_blk(&ttmp_idx,val+cs_n("ttmp"),c->kw_e);
      if(r1){
        if(ttmp_idx>11){
          k_s8 r2=MSG("ttmp index above 11\n");
          if(r2) r0=r2;
          goto exit;
        }
        i_f->val=(k_u16)ttmp_idx+112;
        r0=0;
        goto exit;
      }
      k_s8 r2=MSG("ttmp index is not decimal\n");
      if(r2) r0=r2;
      goto exit;
    }
  }
  if(!u_a_strncmp(val,_("m0"),val_sz)){
    i_f->val=124;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("exec_lo"),val_sz)){
    i_f->val=126;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("exec_hi"),val_sz)){
    i_f->val=127;
    r0=0;goto exit;
  }

  if(val_sz==1||val_sz==2){
    k_u8 integer;
    k_ut r1=u_a_strict_dec2u8_blk(&integer,val,c->kw_e);
    if(r1){
      if(integer>64){
        k_s8 r2=MSG("integer above 64\n");
        if(r2) r0=r2;
        goto exit;
      }
      i_f->val=(k_u16)integer+128;
      r0=0;
      goto exit;
    }
  }
  if(val[0]=='-'&&(val_sz==2||val_sz==3)){
    k_u8 ninteger;
    k_ut r1=u_a_strict_dec2u8_blk(&ninteger,val+1,c->kw_e);
    if(r1){
      if(ninteger>64||ninteger==0){
        k_s8 r2=MSG("negative integer below 64 or is 0\n");
         if(r2) r0=r2;
        goto exit;
      }
      i_f->val=(k_u16)ninteger+193;
      r0=0;
      goto exit;
    }
  }
  if(!u_a_strncmp(val,_("0.5"),val_sz)){
    i_f->val=240;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("-0.5"),val_sz)){
    i_f->val=241;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("1.0"),val_sz)){
    i_f->val=242;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("-1.0"),val_sz)){
    i_f->val=243;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("2.0"),val_sz)){
    i_f->val=244;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("-2.0"),val_sz)){
    i_f->val=245;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("4.0"),val_sz)){
    i_f->val=246;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("-4.0"),val_sz)){
    i_f->val=247;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("vccz"),val_sz)){
    i_f->val=251;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("execz"),val_sz)){
    i_f->val=252;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("scc"),val_sz)){
    i_f->val=253;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("literal_constant"),val_sz)){
    i_f->val=255;
    r0=0;goto exit;
  }
  if(val_sz>=2&&val[0]=='v'){
    k_u8 vgpr_idx;
    k_ut r1=u_a_strict_dec2u8_blk(&vgpr_idx,val+1,c->kw_e);
    if(r1){
      i_f->val=(k_u16)vgpr_idx+256;
      r0=0;
      goto exit;
    }
  }
 
  k_s8 r2=MSG("has an unknown scc value\n");
  if(r2) r0=r2;

exit:
  return r0; 
}

static k_s8 f_bool(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s8 r0=0;

  k_ul val_sz=c->kw_e-val+1;
  if(val_sz!=1){r0=CMINGCNASM_ERR;goto exit;}

       if(val[0]=='0') i_f->val=0;
  else if(val[0]=='1') i_f->val=1;
  else r0=CMINGCNASM_ERR;

exit:
  if(r0!=0){
    k_s8 r1=MSG("has an invalid boolean value\n");
    if(!r1) r0=r1;
  }
  return r0;
}

static k_s8 f_vgpr(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s8 r0=0;

  k_ul val_sz=c->kw_e-val+1;
  if(val_sz<2||val[0]!='v'){
    r0=MSG("has an unknown vgpr value\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  k_u8 vgpr_idx;
  k_ut r1=u_a_strict_dec2u8_blk(&vgpr_idx,val+1,c->kw_e);
  if(!r1){
    r0=MSG("has an invalid vgpr index\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  i_f->val=(k_u16)vgpr_idx;

exit:
  return r0;
}

static k_s8 f_sgpr_mod_4(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s8 r0=0;

  k_ul val_sz=c->kw_e-val+1;
  if(val_sz<2&&val[0]!='s'){
    r0=MSG("has an unknown sgpr value\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  k_u8 sgpr_idx;
  k_ut r1=u_a_strict_dec2u8_blk(&sgpr_idx,val+1,c->kw_e);
  if(!r1){
    r0=MSG("has an invalid sgpr index\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  if(sgpr_idx>103){
    r0=MSG("sgpr index above 103\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  if(sgpr_idx%4){
    r0=MSG("sgpr index in not 4 register aligned\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }
  
  i_f->val=(k_u16)(sgpr_idx>>2);

exit:
  return r0;
}

static k_s8 f_u16(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s8 r0=0;
  
  k_ul val_sz=c->kw_e-val+1;
  if(val_sz>=3){
    if(val[0]=='0'&&val[1]=='b'){//binary value
      k_ut r1=u_a_strict_bin2u16_blk(&i_f->val,&val[2],c->kw_e);
      if(!r1){
        r0=MSG("has not a valid binary value\n");
        if(!r0) r0=CMINGCNASM_ERR;
      }
      goto exit;
    }

    if(val[0]=='0'&&val[1]=='x'){//hexadecimal value
      k_ut r1=u_a_strict_hex2u16_blk(&i_f->val,&val[2],c->kw_e);
      if(!r1){
        r0=MSG("has not a valid hexadecimal value\n");
        r0=CMINGCNASM_ERR;
      }
      goto exit;
    }
  }

  //decimal
  k_ut r1=u_a_strict_dec2u16_blk(&i_f->val,val,c->kw_e);
  if(!r1){
    r0=MSG("has not a valid decimal value\n");
    if(!r0) r0=CMINGCNASM_ERR;
  }

exit:
  return r0;
}

static k_s8 f_tgt(struct ctx *c,k_u8 f,struct i_f *i_f,k_u8 *val)
{
  k_s r0=CMINGCNASM_ERR;//in error state by default

  k_ul val_sz=c->kw_e-val+1;

  if(val_sz==cs_n("mrt")+1){
    if(!u_a_strncmp(val,_("mrt"),cs_n("mrt"))){
      k_u8 mrt_idx;
      k_ut r1=u_a_strict_dec2u8_blk(&mrt_idx,val+cs_n("mrt"),c->kw_e);
      if(r1){
        if(mrt_idx>=8){
          r0=MSG("mrt index above 7\n");
          if(!r0) r0=CMINGCNASM_ERR;goto exit;
        }
        i_f->val=(k_u16)mrt_idx;
        r0=0;
        goto exit;
      }
      r0=MSG("mrt index is not decimal\n");
      if(!r0) r0=CMINGCNASM_ERR;goto exit;
    }
  }
  if(!u_a_strncmp(val,_("mrtz"),val_sz)){
    i_f->val=8;
    r0=0;goto exit;
  }
  if(!u_a_strncmp(val,_("null"),val_sz)){
    i_f->val=9;
    r0=0;goto exit;
  }
  if(val_sz==cs_n("pos")+1){
    if(!u_a_strncmp(val,_("pos"),cs_n("pos"))){
      k_u8 pos_idx;
      k_ut r1=u_a_strict_dec2u8_blk(&pos_idx,val+cs_n("pos"),c->kw_e);
      if(r1){
        if(pos_idx>=4){
          r0=MSG("pos index above 3\n");
          if(!r0) r0=CMINGCNASM_ERR;goto exit;
        }
        i_f->val=(k_u16)pos_idx+12;
        r0=0;
        goto exit;
      }
      r0=MSG("pos index is not decimal\n");
      if(!r0) r0=CMINGCNASM_ERR;goto exit;
    }
  }
  if(val_sz>=cs_n("param")+1){
    if(!u_a_strncmp(val,_("param"),cs_n("param"))){
      k_u8 param_idx;
      k_ut r1=u_a_strict_dec2u8_blk(&param_idx,val+cs_n("param"),c->kw_e);
      if(r1){
        if(param_idx>=32){
          r0=MSG("param index above 31\n");
          if(!r0) r0=CMINGCNASM_ERR;goto exit;
        }
        i_f->val=(k_u16)param_idx+32;
        r0=0;
        goto exit;
      }
      r0=MSG("param index is not decimal\n");
      if(!r0) r0=CMINGCNASM_ERR;goto exit;
    }
  }

  r0=MSG("has an unknown value\n");
  if(!r0) r0=CMINGCNASM_ERR;
 
exit:
  return r0;
}
#undef MSG

static k_s8 (*fs_val_parser[F_INVALID])(struct ctx *c,k_u8 f,struct i_f *i_f,
                                                                    k_u8 *val)={
  0,           //F_SSRC0   
  0,           //F_SSRC1   
  0,           //F_SDST    
  0,           //F_SIMM16  
  0,           //F_OFFSET  
  0,           //F_IMM     
  0,           //F_SBASE   
  f_scc,       //F_SRC0    
  f_scc,       //F_SRC1    
  f_scc,       //F_SRC2    
  f_vgpr,      //F_VSRC0   
  f_vgpr,      //F_VSRC1   
  f_vgpr,      //F_VSRC2   
  f_vgpr,      //F_VSRC3   
  f_vgpr,      //F_VDST    
  f_u16,       //F_ABS     
  f_bool,      //F_CLAMP   
  f_u16,       //F_OMOD    
  f_u16,       //F_NEG     
  0,           //F_VSRC    
  0,           //F_ATTRCHAN
  0,           //F_ATTR    
  0,           //F_OFFSET0 
  0,           //F_OFFSET1 
  0,           //F_GDS     
  0,           //F_ADDR    
  0,           //F_DATA0   
  0,           //F_DATA1   
  f_bool,      //F_OFFEN   
  f_bool,      //F_IDXEN   
  0,           //F_GLC     
  0,           //F_ADDR64  
  0,           //F_LDS     
  0,           //F_VADDR   
  f_vgpr,      //F_VDATA   
  f_sgpr_mod_4,//F_SRSRC   
  0,           //F_SLC     
  0,           //F_TFE     
  f_scc,       //F_SOFFSET 
  0,           //F_DFMT    
  0,           //F_NFMT    
  0,           //F_DMASK   
  0,           //F_UNORM   
  0,           //F_DA      
  0,           //F_R128    
  0,           //F_LWE     
  0,           //F_SSAMP   
  f_u16,       //F_EN      
  f_tgt,       //F_TGT     
  f_bool,      //F_COMPR   
  f_bool,      //F_DONE    
  f_bool,      //F_VM      
  f_u16,       //F_VM_CNT  
  f_u16,       //F_EXP_CNT 
  f_u16        //F_LGKM_CNT
};

#define MSG(x,...) msg(c->msgs,_("error:source parse:%s:%d:pp(%d:%d):" x),\
c->src_pathname,c->src_l,c->l,c->p-c->l_s+1,##__VA_ARGS__)
static k_s8 l_kw_f(struct ctx *c)
{
  k_s8 r=0;

  //make room for a new field in the instruction
  k_u8 i_f;
  for(i_f=0;i_f<FS_MAX;++i_f)
    if(c->is[c->is_last].fs[i_f].f==F_INVALID) break;
  if(i_f==FS_MAX){
    r=MSG("no more room for field in instruction\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
  if(i_f<(FS_MAX-1)) c->is[c->is_last].fs[i_f+1].f=F_INVALID;

  //locate value separator '='
  k_u8 *mnemonic_e=c->p;
  while(mnemonic_e<=c->kw_e&&*mnemonic_e!='=') ++mnemonic_e;
  if(mnemonic_e>=c->kw_e){
    r=MSG("field value separator not found or missing value\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
  mnemonic_e--;

  //find proper field index
  k_ul mnemonic_sz=mnemonic_e-c->p+1;
  k_u8 f;
  for(f=0;f<F_INVALID;++f)
    if(!u_a_strncmp(c->p,fs_mnemonic[f],mnemonic_sz)) break;
  if(f==F_INVALID){
    r=MSG("field mnemonic not found\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }

  c->is[c->is_last].fs[i_f].f=f;

  //parse value
  if(!fs_val_parser[f]){
    r=MSG("%s field hasn't a parser\n",fs_mnemonic[f]);
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
  //call the parser
  r=fs_val_parser[f](c,f,&c->is[c->is_last].fs[i_f],mnemonic_e+1+1);
exit:
  return r;
}
#undef MSG

static void l_next_init(struct ctx *c)
{
    ++c->l;
    ++c->src_l;
    c->l_s=c->p;
    c->l_e=c->p;
    while(c->l_e<c->src+c->src_sz&&*c->l_e!='\n') ++c->l_e;
    if(c->l_e==c->src+c->src_sz) c->flgs|=LAST_LINE_PROCESSING;
}

#define MSG(x) msg(c->msgs,_("error:source parse:pp(%d):line preprocessor" \
" directive:" x),c->l)
static k_s8 pp(struct ctx *c)
{
  k_s8 r0=0;

  k_u8 *p=c->p+1;//skip '#'

  //skip white space(s) till the source file line number
  while(p<c->l_e){
    if(*p!=' ') break;
    ++p;
  }
  if(p==c->l_e){
    r0=MSG("missing source file line number\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }

  //get the source file line number keyword
  k_u8 *pp_kw_e=p;
  while(pp_kw_e<c->l_e){
    if(!u_a_is_digit(*pp_kw_e)) break;
    ++pp_kw_e;
  }
  if(pp_kw_e==c->l_e){
    r0=MSG("missing source file pathname\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }
  pp_kw_e--;
 
  //convert the source file line number keyword 
  k_u64 src_l;
  k_ut r1=u_a_strict_dec2u64_blk(&src_l,p,pp_kw_e);
  if(!r1){
    r0=MSG("does not have a valide line number\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }
  p=pp_kw_e+1;
  c->src_l=(k_s32)src_l-1;//store the source line number, swallowing itself

  //skip white space(s) till the source file pathname
  while(p<c->l_e){
    if(*p!=' ') break;
    ++p;
  }
  if(p==c->l_e||*p!='"'){//the pathname must be enclosed within '"' chars
    r0=MSG("missing a source file pathname or it's missing the starting \"\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }
  ++p;//skip '"'
 
  //Get the source file pathname, delimitted with '"' char. Escaping is done
  //inside the pathname.
  k_u8 *d=&c->src_pathname[0];
  k_u8 *d_e=&c->src_pathname[0]+SRC_PATHNAME_SZ_MAX;//right after the last char
  while(p<c->l_e&&*p!='"'&&d<d_e){
    if(*p=='\\'){
      if((p+1)==c->l_e||(d+1)==d_e) break;//test ahead
      ++p;
    }
    *d++=*p++; 
  }
  if(*p!='"'){
    r0=MSG("unable to get source pathname\n");
    if(!r0) r0=CMINGCNASM_ERR;goto exit;
  }
  *d=0;
exit:  
  return r0;
}
#undef MSG

k_s8 src_parse(k_u8 *src,k_s32 src_sz,k_u8 *src_pathname_default,struct i **is,
                                           k_s32 *is_last,struct msgs_ctx *msgs)
{
  k_s8 r=0;

  struct ctx c;
  c.src=src;
  c.src_sz=src_sz;
  c.is=0;
  c.is_last=-1;
  c.p=src;
  c.flgs=0;
  c.l=0;
  c.src_l=0;
  u_a_strncpy(&c.src_pathname[0],src_pathname_default,SRC_PATHNAME_SZ_MAX);
  c.src_pathname[SRC_PATHNAME_SZ_MAX]=0;
  c.msgs=msgs;

  do{
    l_next_init(&c);

    if(*c.p=='#'){//preprocessor line
      r=pp(&c);
      if(r!=0) goto exit;
      l_next(&c);
      continue;
    }

    l_blanks_skip(&c);//skip any blanks at the beginning of the line
    if(is_l_e(&c)){//this is a blank line
      l_next(&c);
      continue;
    }

    l_kws_next(&c);//fetch first keyword

    //--------------------------------------------------------------------------
    //label if any
    if(*c.kw_e==':'){//first keyword is a label
      r=l_kw_label(&c);
      if(r!=0) goto exit;

      l_kw_consumed(&c);//skip the processed label

      l_blanks_skip(&c);//skip any blanks after the label

      if(is_l_e(&c)){//label is standalone on its line
        c.flgs|=HAVE_LABEL;
        l_next(&c);
        continue;
      }

      l_kws_next(&c);//fetch second keyword
    }
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //instruction
    r=l_kw_i(&c);//that keyword must be an instruction
    if(r!=0) goto exit;

    c.flgs&=~HAVE_LABEL;//reset label presence

    l_kw_consumed(&c);//skip the processed instruction

    l_blanks_skip(&c);//try to reach the first instruction field
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //processing loop of fields
    while(!is_l_e(&c)){
      l_kws_next(&c);//fetch next keyword

      r=l_kw_f(&c);//that keyword must be a field
      if(r!=0) goto exit;

      l_kw_consumed(&c);//skip the processed field

      l_blanks_skip(&c);//try to reach next field
    }
    //--------------------------------------------------------------------------

    l_next(&c);
  }while(!(c.flgs&LAST_LINE_PROCESSING));

  *is=&c.is[0];
  *is_last=c.is_last;

exit:
  if(r!=0) is_unmap(c.is,c.is_last,msgs);
  return r;
}

