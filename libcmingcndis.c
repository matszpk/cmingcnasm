//******************************************************************************
//this code is protected by the GNU affero GPLv3 with a lesser exception
//for video games
//author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//                        <digital.ragnarok AT gmail dot com>
//******************************************************************************


//**IMPORTANT NOTICE: do *NOT* factor the code. It still misses features.


//------------------------------------------------------------------------------
//compiler stuff
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//ulinux stuff
//------------------------------------------------------------------------------
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/endian.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//public
//------------------------------------------------------------------------------
#include "cmingcndis.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//private
//------------------------------------------------------------------------------
#include "msgs.h"
#define IS_C
#include "is.h"
#undef IS_C
//------------------------------------------------------------------------------

#define _(x) ((k_u8*)x)
#define cs_n(x) sizeof(x)-1

struct ctx{
  void *m;
  void *i;

  k_s32 src_sz_max;
  k_u8 **src;
  k_s32 *src_sz;
  struct msgs_ctx *msgs;
};

#define MSG(x) msg(c->msgs,_("error:" x),addr)
static k_s8 src_grow(struct ctx *c,k_l len)
{
  k_s8 r=0;
  k_l addr;

  k_s32 old_len=*c->src_sz;
  k_s32 new_len=old_len+len;

  if(old_len==0){//first allocation, then mmapping
    addr=sysc(mmap,6,0,new_len,K_PROT_READ|K_PROT_WRITE,
                                            K_MAP_PRIVATE|K_MAP_ANONYMOUS,-1,0);
    if(K_ISERR(addr)){
      r=MSG("mmap(%ld):unable to mmap source code buffer\n");
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
  }else{//grow mapping
    addr=sysc(mremap,5,*c->src,old_len,new_len,K_MREMAP_MAYMOVE,0);
    if(K_ISERR(addr)){
      r=MSG("mremap(%ld):unable to remap source code buffer\n");
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
  }
  *c->src=(k_u8*)addr;
  *c->src_sz=new_len;
exit:
  return r; 
}
#undef MSG

static k_s8 src_unmap(struct ctx *c)
{
  if(*c->src_sz==0) return 0;//nothing to unmap

  k_l r=sysc(munmap,2,*c->src,*c->src_sz);
  if(K_ISERR(r)){
      r=(k_l)msg(c->msgs,_("error:munmap(%ld):unable to unmap source code"
                                                                " buffer\n"),r);
      if(!r) r=CMINGCNDIS_ERR;
  }
  return (k_s8)r;
}

static k_s8 sopp(struct ctx *c)
{
  k_s8 r;

  k_u32 i=u_le322cpu(*(k_u32*)(c->i));

  k_u8 op=(i>>16)&0x7f;

  struct i_mnemonic_map *map=&i_mnemonic_maps[0];
  while(map->mnemonic!=0){
    if(map->fmts&BIT(FMT_SOPP)
                         &&(map->op_base+fmt_op_offset(map->fmts,FMT_SOPP))==op)
      break;
    ++map;
  }
  if(map->mnemonic==0){
    r=msg(c->msgs,_("error:0x%lx:sopp mnemonic not found\n"),c->i-c->m);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }

  if(op==12){//the s_waitcnt sopp has 3 subfields instead af 1 simm16 field
    k_u8 vm_cnt=i&0xf;
    k_u8 exp_cnt=(i>>4)&0x7;
    k_u8 lgkm_cnt=(i>>8)&0x1f;

    k_u8 *i_str=_("\t%s %s=%u %s=%u %s=%u\n");
    k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,fs_mnemonic[F_VM_CNT],vm_cnt,
                                              fs_mnemonic[F_EXP_CNT],exp_cnt,
                                              fs_mnemonic[F_LGKM_CNT],lgkm_cnt);
    if(len<=0){
      r=msg(c->msgs,_("error:0x%lx:sopp:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
    r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start
    if(r!=0) goto exit;

    //do swallow each time, except the first time, the \0
    len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
                                              map->mnemonic,
                                              fs_mnemonic[F_VM_CNT],vm_cnt,
                                              fs_mnemonic[F_EXP_CNT],exp_cnt,
                                              fs_mnemonic[F_LGKM_CNT],lgkm_cnt);
    if(len<0){
      r=msg(c->msgs,_("error:0x%lx:sopp:%s unable to print instruction string"
                           " in source code buffer\n"),c->i-c->m,map->mnemonic);
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
  }else{
    k_u16 simm16=i&0xffff;

    k_u8 *i_str=_("\t%s %s=%u\n");
    k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,fs_mnemonic[F_SIMM16],simm16);
    if(len<=0){
      r=msg(c->msgs,_("error:0x%lx:sopp:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
    r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start;
    if(r!=0) goto exit;

    //do swallow each time, except the first time, the \0
    len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
                                    map->mnemonic,fs_mnemonic[F_SIMM16],simm16);
    if(len<0){
      r=msg(c->msgs,_("error:0x%lx:sopp:%s unable to print instruction string"
                           " in source code buffer\n"),c->i-c->m,map->mnemonic);
      if(!r) r=CMINGCNDIS_ERR;goto exit;
    }
  }
  r=sizeof(i);
exit:
  return r;
}

static k_s8 sopc(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:sopc unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 sop1(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:sop1 unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 vopc(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:vopc unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

#define SCC_STR_BUF_SZ sizeof("exec_lo")
static void scc_str(k_u8 *d,k_u16 v)
{
  if(v<=103) u_a_snprintf(d,SCC_STR_BUF_SZ,_("s%u"),v);
  else if(v==106) u_a_strncpy(d,_("vcc_lo"),SCC_STR_BUF_SZ);
  else if(v==107) u_a_strncpy(d,_("vcc_hi"),SCC_STR_BUF_SZ);
  else if(v==108) u_a_strncpy(d,_("tba_lo"),SCC_STR_BUF_SZ);
  else if(v==109) u_a_strncpy(d,_("tba_hi"),SCC_STR_BUF_SZ);
  else if(v==110) u_a_strncpy(d,_("tma_lo"),SCC_STR_BUF_SZ);
  else if(v==111) u_a_strncpy(d,_("tma_hi"),SCC_STR_BUF_SZ);
  else if(112<=v&&v<=123) u_a_snprintf(d,SCC_STR_BUF_SZ,_("ttmp%u"),v-112);
  else if(v==124) u_a_strncpy(d,_("m0"),SCC_STR_BUF_SZ);
  else if(v==126) u_a_strncpy(d,_("exec_lo"),SCC_STR_BUF_SZ);
  else if(v==127) u_a_strncpy(d,_("exec_hi"),SCC_STR_BUF_SZ);
  else if(129<=v&&v<=192) u_a_snprintf(d,SCC_STR_BUF_SZ,_("%u"),v-129+1);
  else if(193<=v&&v<=208) u_a_snprintf(d,SCC_STR_BUF_SZ,_("-%u"),v-193+1);
  else if(v==240) u_a_strncpy(d,_("0.5"),SCC_STR_BUF_SZ);
  else if(v==241) u_a_strncpy(d,_("-0.5"),SCC_STR_BUF_SZ);
  else if(v==242) u_a_strncpy(d,_("1.0"),SCC_STR_BUF_SZ);
  else if(v==243) u_a_strncpy(d,_("-1.0"),SCC_STR_BUF_SZ);
  else if(v==244) u_a_strncpy(d,_("2.0"),SCC_STR_BUF_SZ);
  else if(v==245) u_a_strncpy(d,_("-2.0"),SCC_STR_BUF_SZ);
  else if(v==246) u_a_strncpy(d,_("4.0"),SCC_STR_BUF_SZ);
  else if(v==247) u_a_strncpy(d,_("-4.0"),SCC_STR_BUF_SZ);
  else if(v==251) u_a_strncpy(d,_("vccz"),SCC_STR_BUF_SZ);
  else if(v==252) u_a_strncpy(d,_("execz"),SCC_STR_BUF_SZ);
  else if(v==253) u_a_strncpy(d,_("scc"),SCC_STR_BUF_SZ);
  else if(v==254) u_a_strncpy(d,_("lds_direct"),SCC_STR_BUF_SZ);
  else if(v==255) u_a_strncpy(d,_("literal_constant"),SCC_STR_BUF_SZ);
  else if(256<=v&&v<=511) u_a_snprintf(d,SCC_STR_BUF_SZ,_("v%u"),v-256);
}

static k_s8 vop1(struct ctx *c)
{
  k_s8 r;

  k_u32 i=u_le322cpu(*(k_u32*)(c->i));

  k_u16 src0=i&0x1ff;
  k_u8 op=(i>>9)&0xff;
  k_u8 vdst=(i>>17)&0xff;

  k_u8 src0_str[SCC_STR_BUF_SZ];
  scc_str(&src0_str[0],src0); 

  struct i_mnemonic_map *map=&i_mnemonic_maps[0];
  while(map->mnemonic!=0){
    if(map->fmts&BIT(FMT_VOP1)
                         &&(map->op_base+fmt_op_offset(map->fmts,FMT_VOP1))==op)
      break;
    ++map;
  }
  if(map->mnemonic==0){
    r=msg(c->msgs,_("error:0x%lx:vop1 mnemonic not found\n"),c->i-c->m);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }

  k_u8 *i_str=_("\t%s %s=%s %s=v%u\n");
  k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,fs_mnemonic[F_SRC0],src0_str,
                                                      fs_mnemonic[F_VDST],vdst);
  if(len<=0){
    r=msg(c->msgs,_("error:0x%lx:vop1:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start
  if(r!=0) goto exit;

  //do swallow each time, except the first time, the \0
  len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
           map->mnemonic,fs_mnemonic[F_SRC0],src0_str,fs_mnemonic[F_VDST],vdst);
  if(len<0){
    r=msg(c->msgs,_("error:0x%lx:vop1:%s unable to print instruction string in"
                              " source code buffer\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=sizeof(i);
exit:
  return r;
}

static k_s8 vop3(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:vop3 unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 vintrp(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:vintrp unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 ds(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:ds unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 mubuf(struct ctx *c)
{
  k_s8 r;

  k_u64 i=u_le642cpu(*(k_u64*)(c->i));

  k_u16 offset=i&0xfff;
  k_u8 offen=(i>>12)&1;
  k_u8 idxen=(i>>13)&1;
  k_u8 glc=(i>>14)&1;
  k_u8 addr64=(i>>15)&1;
  k_u8 lds=(i>>16)&1;
  k_u8 op=(i>>18)&0xef;
  k_u8 vaddr=(i>>32)&0xff;
  k_u8 vdata=(i>>40)&0xff;
  k_u8 srsrc=(i>>48)&0x1f;
  k_u8 slc=(i>>54)&1;
  k_u8 tfe=(i>>55)&1;

  struct i_mnemonic_map *map=&i_mnemonic_maps[0];
  while(map->mnemonic!=0){
    if(map->fmts&BIT(FMT_MUBUF)
                        &&(map->op_base+fmt_op_offset(map->fmts,FMT_MUBUF))==op)
      break;
    ++map;
  }
  if(map->mnemonic==0){
    r=msg(c->msgs,_("error:0x%lx:mubuf mnemonic not found\n"),c->i-c->m);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }

  k_u8 *i_str=_("\t%s %s=%u %s=%u %s=%u %s=%u %s=%u %s=%u %s=v%u %s=v%u %s=s%u" \
                                                              " %s=%u %s=%u\n");
  k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,fs_mnemonic[F_OFFSET],offset,
                       fs_mnemonic[F_OFFEN],offen,fs_mnemonic[F_IDXEN],idxen,
                       fs_mnemonic[F_GLC],glc,fs_mnemonic[F_ADDR64],addr64,
                       fs_mnemonic[F_LDS],lds,fs_mnemonic[F_VADDR],vaddr,
                       fs_mnemonic[F_VDATA],vdata,fs_mnemonic[F_SRSRC],srsrc<<2,
                       fs_mnemonic[F_SLC],slc,fs_mnemonic[F_TFE],tfe);
  if(len<=0){
    r=msg(c->msgs,_("error:0x%lx:mubuf:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start
  if(r!=0) goto exit;

  //do swallow each time, except the first time, the \0
  len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
                       map->mnemonic,
                       fs_mnemonic[F_OFFSET],offset,fs_mnemonic[F_OFFEN],offen,
                       fs_mnemonic[F_IDXEN],idxen,fs_mnemonic[F_GLC],glc,
                       fs_mnemonic[F_ADDR64],addr64,fs_mnemonic[F_LDS],lds,
                       fs_mnemonic[F_VADDR],vaddr,fs_mnemonic[F_VDATA],vdata,
                       fs_mnemonic[F_SRSRC],srsrc<<2,fs_mnemonic[F_SLC],slc,
                       fs_mnemonic[F_TFE],tfe);
  if(len<0){
    r=msg(c->msgs,_("error:0x%lx:mubuf:%s unable to print instruction string in"
                              " source code buffer\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=sizeof(i);
exit:
  return r;
}

static k_s8 mtbuf(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:mtbuf unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 mimg(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:mimg unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 export(struct ctx *c)
{
  k_s8 r;

  k_u64 i=u_le642cpu(*(k_u64*)(c->i));

  k_u8 en=i&0xf;

  k_u8 tgt=(i>>4)&0x3f;
  k_u8 tgt_str[sizeof("paramXX")];
  if(tgt<=7)
    u_a_snprintf(&tgt_str[0],sizeof("paramXX"),_("mrt%u"),tgt);
  else if(tgt==8)
    u_a_strncpy(&tgt_str[0],_("mtrz"),sizeof("paramXX"));
  else if(tgt==9)
    u_a_strncpy(&tgt_str[0],_("null"),sizeof("paramXX"));
  else if(12<=tgt&&tgt<=15)
    u_a_snprintf(tgt_str,sizeof("paramXX"),_("pos%u"),tgt-12);
  else if(32<=tgt&&tgt<=63)
    u_a_snprintf(&tgt_str[0],sizeof("paramXX"),_("param%u"),tgt-32);
    
  k_u8 compr=(i>>10)&1;
  k_u8 done=(i>>11)&1;
  k_u8 vm=(i>>12)&1;
  k_u8 vsrc0=(i>>32)&0xff;
  k_u8 vsrc1=(i>>40)&0xff;
  k_u8 vsrc2=(i>>48)&0xff;
  k_u8 vsrc3=(i>>56)&0xff;

  struct i_mnemonic_map *map=&i_mnemonic_maps[0];
  while(map->mnemonic!=0){
    if(map->fmts&BIT(FMT_EXP)) break;//no opcode
    ++map;
  }
  if(map->mnemonic==0){
    r=msg(c->msgs,_("error:0x%lx:exp mnemonic not found\n"),c->i-c->m);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }

  k_u8 *i_str=_("\t%s %s=0x%x %s=%s %s=%u %s=%u %s=%u %s=v%u %s=v%u %s=v%u"
                                                                   " %s=v%u\n");
  k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,
                       fs_mnemonic[F_EN],en,fs_mnemonic[F_TGT],tgt_str,
                       fs_mnemonic[F_COMPR],compr,fs_mnemonic[F_DONE],done,
                       fs_mnemonic[F_VM],vm,fs_mnemonic[F_VSRC0],vsrc0,
                       fs_mnemonic[F_VSRC1],vsrc1,fs_mnemonic[F_VSRC2],vsrc2,
                       fs_mnemonic[F_VSRC3],vsrc3);
  if(len<=0){
    r=msg(c->msgs,_("error:0x%lx:exp:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start;
  if(r!=0) goto exit;

  //do swallow each time, except the first time, the \0
  len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
                          map->mnemonic,
                          fs_mnemonic[F_EN],en,fs_mnemonic[F_TGT],tgt_str,
                          fs_mnemonic[F_COMPR],compr,fs_mnemonic[F_DONE],done,
                          fs_mnemonic[F_VM],vm,fs_mnemonic[F_VSRC0],vsrc0,
                          fs_mnemonic[F_VSRC1],vsrc1,fs_mnemonic[F_VSRC2],vsrc2,
                          fs_mnemonic[F_VSRC3],vsrc3);
  if(len<0){
    r=msg(c->msgs,_("error:0x%lx:exp:%s unable to print instruction string in"
                              " source code buffer\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=sizeof(i);
exit:
  return r;
}

static k_s8 smrd(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:smrd unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 sopk(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:sopk unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 sop2(struct ctx *c)
{
  k_s8 r=msg(c->msgs,_("error:0x%lx:sop2 unimplemented\n"),c->i-c->m);
  if(!r) r=CMINGCNDIS_ERR;
  return r;
}

static k_s8 vop2(struct ctx *c)
{
  k_s8 r;

  k_u32 i=u_le322cpu(*(k_u32*)(c->i));

  k_u16 src0=i&0x1ff;
  k_u8 vsrc1=(i>>0)&0xff;
  k_u8 vdst=(i>>17)&0xff;
  k_u8 op=(i>>25)&0x3f;

  k_u8 src0_str[SCC_STR_BUF_SZ];
  scc_str(&src0_str[0],src0); 

  struct i_mnemonic_map *map=&i_mnemonic_maps[0];
  while(map->mnemonic!=0){
    if(map->fmts&BIT(FMT_VOP2)
                         &&(map->op_base+fmt_op_offset(map->fmts,FMT_VOP2))==op)
      break;
    ++map;
  }
  if(map->mnemonic==0){
    r=msg(c->msgs,_("error:0x%lx:vop2 mnemonic not found\n"),c->i-c->m);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }

  k_u8 *i_str=_("\t%s %s=%s %s=v%u %s=v%u\n");
  k_l len=u_a_snprintf(0,0,i_str,map->mnemonic,fs_mnemonic[F_SRC0],src0_str,
                           fs_mnemonic[F_VSRC1],vsrc1,fs_mnemonic[F_VDST],vdst);
  if(len<=0){
    r=msg(c->msgs,_("error:0x%lx:vop2:%s unable to evaluate the resulting"
                              " instruction string\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=src_grow(c,len+(*c->src_sz?0:1));//don't miss the \0 at the start
  if(r!=0) goto exit;

  //do swallow each time, except the first time, the \0
  len=u_a_snprintf(&(*c->src)[*c->src_sz-len-(*c->src_sz?1:0)],len+1,i_str,
           map->mnemonic,fs_mnemonic[F_SRC0],src0_str,fs_mnemonic[F_VDST],vdst);
  if(len<0){
    r=msg(c->msgs,_("error:0x%lx:vop2:%s unable to print instruction string in"
                              " source code buffer\n"),c->i-c->m,map->mnemonic);
    if(!r) r=CMINGCNDIS_ERR;goto exit;
  }
  r=sizeof(i);
exit:
  return r;
}

//return how long the machine instruction was
static k_s8 i_dis(struct ctx *c)
{
   k_s8 r;
   k_u32 i0=u_le322cpu(*(k_u32*)(c->i));
  
        if((i0&0xff800000)==0xbf800000) r=sopp(c);
   else if((i0&0xff800000)==0xbf000000) r=sopc(c);
   else if((i0&0xff800000)==0xbe800000) r=sop1(c);
   else if((i0&0xfe000000)==0x78000000) r=vopc(c);
   else if((i0&0xfe000000)==0x7e000000) r=vop1(c);
   else if((i0&0xfc000000)==0xd0000000) r=vop3(c);
   else if((i0&0xfc000000)==0xa8000000) r=vintrp(c);
   else if((i0&0xfc000000)==0xd8000000) r=ds(c);
   else if((i0&0xfc000000)==0xe0000000) r=mubuf(c);
   else if((i0&0xfc000000)==0xe8000000) r=mtbuf(c);
   else if((i0&0xfc000000)==0xf0000000) r=mimg(c);
   else if((i0&0xfc000000)==0xf8000000) r=export(c);
   else if((i0&0xf8000000)==0xc0000000) r=smrd(c);
   else if((i0&0xf0000000)==0xb0000000) r=sopk(c);
   else if((i0&0xc0000000)==0x20000000) r=sop2(c);
   else if((i0&0x80000000)==0x00000000) r=vop2(c);
   else{
     r=msg(c->msgs,_("error:0x%lx:instruction unknown\n"),c->i-c->m);
     if(!r) r=CMINGCNDIS_ERR;
   }
   return r;
}

k_s8 cmingcndis_dis(void *m,
                    k_s32 m_sz,
                    k_s32 src_sz_max,
                    k_u8 **src,
                    k_s32 *src_sz,
                    k_s32 msgs_sz_max,
                    k_u8 **msgs,
                    k_s32 *msgs_sz)
{
  k_s8 r0=0;
  k_s8 r1=0;

  if(msgs&&msgs_sz){
    *msgs=0;
    *msgs_sz=0;
  }
  struct msgs_ctx msgs_c;
  msgs_c.sz_max=msgs_sz_max;
  msgs_c.msgs=msgs;
  msgs_c.sz=msgs_sz;

  if(!m||m_sz<=0){
    r0=msg(&msgs_c,_("machine code empty(p=0x%p:sz=%d)\n"),m,m_sz);
    if(!r0) r0=CMINGCNDIS_ERR;goto exit;
  }
  if(src_sz_max<=0){
    r0=msg(&msgs_c,_("invalid maximum size of source code(%d)\n"),src_sz_max);
    if(!r0) r0=CMINGCNDIS_ERR;goto exit;
  }

  struct ctx c;
  c.m=m;
  c.src_sz_max=src_sz_max;
  c.src=src;
  *c.src=0;
  c.src_sz=src_sz;
  *c.src_sz=0;
  c.msgs=&msgs_c;

  //start work here
  c.i=m;
  void *m_e=m+m_sz;//points right after the last byte
  while(c.i<m_e){
    r0=i_dis(&c);
    if(r0<0) goto err_unmap_src;
    c.i+=r0;
  }
  (*c.src_sz)--;//swallow the snprint \0
  goto exit;

err_unmap_src:
  r1=src_unmap(&c);
  if(r1) r0=r1;

exit:
  return r0;
}
