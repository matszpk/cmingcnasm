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
#include <ulinux/types.h>
#include <ulinux/sysc.h>
#include <ulinux/error.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/endian.h>
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

static k_s8 bytes_new(k_u8 bytes,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=0;
  k_l addr;

  k_s32 new_len;
  if(!*m_sz){//first allocation, then mmapping
    new_len=bytes;
    addr=sysc(mmap,6,0,bytes,K_PROT_READ|K_PROT_WRITE,
                                            K_MAP_PRIVATE|K_MAP_ANONYMOUS,-1,0);
    if(K_ISERR(addr)){
      r=msg(msgs,_("error:machine emit:mmap(%ld):unable to mmap machine"
                                                   " instruction slab\n"),addr);
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
  }else{//remapping
    new_len=*m_sz+bytes;
    if(new_len>m_sz_max){
      r=msg(msgs,_("error:machine emit:mremap:unable to remap machine"
                             " instruction slab because max sz was reached\n"));
      if(!r) r=CMINGCNASM_ERR;goto exit;     
    }

    addr=sysc(mremap,5,*m,*m_sz,new_len,K_MREMAP_MAYMOVE,0);
    if(K_ISERR(addr)){
      r=msg(msgs,_("error:machine emit:mremap(%ld):unable to remap machine"
                                                   " instruction slab\n"),addr);
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
  }

  *m=(void*)addr;
  *m_sz=new_len;
exit:
  return r; 
}

static k_s8 le32_new(k_s32 m_sz_max,void **m,k_s32 *m_sz,struct msgs_ctx *msgs)
{
  return bytes_new(sizeof(k_u32),m_sz_max,m,m_sz,msgs);
}

static k_s8 le64_new(k_s32 m_sz_max,void **m,k_s32 *m_sz,struct msgs_ctx *msgs)
{
  return bytes_new(sizeof(k_u64),m_sz_max,m,m_sz,msgs);
}

static k_s8 unmap(void *m,k_s32 m_sz,struct msgs_ctx *msgs)
{
  if(!m_sz) return 0;//nothing to unmap

  k_l r=sysc(munmap,2,m,m_sz);
  if(K_ISERR(r)){
    r=(k_l)msg(msgs,_("error:machine emit:munmap(%ld):unable to unmap machine"
                                                       "instruction slab\n"),r);
    if(!r) r=CMINGCNASM_ERR;
  }
  return (k_s8)r;
}


static k_s8 emit_64(k_u64 m_i_le,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=le64_new(m_sz_max,m,m_sz,msgs);
  if(r!=0) goto exit;

  k_u64 *p=*m+*m_sz-sizeof(m_i_le);
  *p=m_i_le;
exit:
  return r;
}

static k_s8 emit_32(k_u32 m_i_le,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=le32_new(m_sz_max,m,m_sz,msgs);
  if(r!=0) goto exit;

  k_u32 *p=*m+*m_sz-sizeof(m_i_le);
  *p=m_i_le;
exit:
  return r;
}


static struct i_f *f_get(struct i *i,k_u8 f)
{
  k_u8 cur_f;
  for(cur_f=0;cur_f<FS_MAX;++cur_f) if(i->fs[cur_f].f==f) break;
  struct i_f *r=0;
  if(cur_f<FS_MAX) r=&i->fs[cur_f];
  return r;
}

//==============================================================================
//mubuf
static k_s8 mubuf_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
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
      r=msg(msgs,_("error:machine emit:mubuf:%s:%d:pp(%d):%s field illegal in"
      " this instruction\n"),i->src_pathname,i->src_l,i->l,
      fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

#define MSG(x,y) msg(msgs,_(x ":machine emit:mubuf:%s:%d:pp(%d):" y),\
i->src_pathname,i->src_l,i->l)
static k_s8 mubuf(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=mubuf_fs_chk(i,msgs);
  if(r) goto exit;
  
  k_u64 m_i=0;

  k_u8 offset_set=0;
  struct i_f *f=f_get(i,F_OFFSET);
  if(f){
    offset_set=1;
    if(f->val&~0xfff){
      r=MSG("error","offset value too large\n");
      if (!r) r=CMINGCNASM_ERR;goto exit;
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

  m_i|=(k_u64)0b111000<<26;

  k_u8 vaddr_set=0;
  f=f_get(i,F_VADDR);
  if(f){//may target 1 *or 2 consecutive* vgprs, that based on idxen and offen
    vaddr_set=1;
    m_i|=(k_u64)(f->val&0xff)<<32;
  }

  f=f_get(i,F_VDATA);
  if(f) m_i|=(k_u64)(f->val&0xff)<<40;
  
  f=f_get(i,F_SRSRC);
  if(f) m_i|=(k_u64)(f->val&0x1f)<<48;

  f=f_get(i,F_SLC);
  if(f) m_i|=(k_u64)(f->val&1)<<54;

  f=f_get(i,F_TFE);
  if(f) m_i|=(k_u64)(f->val&1)<<55;

  f=f_get(i,F_SOFFSET);
  if(f){//may select a vgpr which would be out of range
    if(f->val&~0xff){
      r=MSG("error","soffset value targets a vgpr\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=(k_u64)(f->val&0xff)<<56;
  }

  //----------------------------------------------------------------------------
  //start instruction scale checks (warnings and errors)
  //----------------------------------------------------------------------------
  //if addr64==1, idxen and offen must be 0
  if(m_i>>15&1&&(m_i>>12&1||m_i>>13&1)){
    r=MSG("error","when addr64==1 then offen and idxen must be 0\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }

  //if offen=1 the content of a set offset will be ignored
  if(m_i>>12&1&&offset_set){
    r=MSG("warning","offen is 1 then content of offset will be ignored\n");
    if(r) goto exit;
  }

  //if offen=0 and idxen=0, setting a value for vaddr in useless
  if(vaddr_set&&!(m_i>>12&1)&&!(m_i>>13&1)){
    r=MSG("warning","setting a value to vaddr is useless unless you set offen"
                                                                 " or idxen\n");
    if(r) goto exit;
  }

  //if offen=1 and idxen=1, better double check vaddr content
  if(m_i>>12&1&&m_i>>13&1){
    r=MSG("warning","offen and idxen set together, double check the content of"
                                             " the 2 vgprs pointed by vaddr\n");
  }
 
  //illegal to set tfe and lds together
  if(m_i>>55&1&&m_i>>16&1){
    r=MSG("error","cannot set tfe and lds together\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }
  //----------------------------------------------------------------------------

  r=emit_64(u_cpu2le64(m_i),m_sz_max,m,m_sz,msgs);

exit:
  return r;
}
#undef MSG
//end mubuf
//==============================================================================

//==============================================================================
//s_waitcnt
static k_s8 s_waitcnt_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
    if(i->fs[i_f].f==F_INVALID) break;

    switch(i->fs[i_f].f){
    case F_VM_CNT:
    case F_EXP_CNT:
    case F_LGKM_CNT:
      break;
    case F_SIMM16:
      r=msg(msgs,_("error:machine emit:s_waitcnt:%s:%d:pp(%d):don't use %s"
                    " field, use vm_cnt exp_cnt lgkm_cnt sub-fields instead\n"),
                       i->src_pathname,i->src_l,i->l,fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
      break;
    default:
      r=msg(msgs,_("error:machine emit:s_waitcnt:%s:%d:pp(%d):%s field illegal"
                        " in this instruction\n"),i->src_pathname,i->src_l,i->l,
                                                     fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

#define MSG(x) msg(msgs,_("error:machine emit:s_waitcnt:%s:%d:pp(%d):" x),\
i->src_pathname,i->src_l,i->l)
static k_s8 s_waitcnt(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=s_waitcnt_fs_chk(i,msgs);
  if(r) goto exit;

  k_u32 m_i=0;

  struct i_f *f=f_get(i,F_VM_CNT);
  if(f){
    if(f->val>0xf){
      r=MSG("vector memory count too large\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=f->val&0xf;
  }

  f=f_get(i,F_EXP_CNT);
  if(f){
    if(f->val>7){
      r=MSG("vgpr export count too large\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
   m_i|=(f->val&7)<<4;
  }

  f=f_get(i,F_LGKM_CNT);
  if(f){
    if(f->val>0x1f){
      r=MSG("LDS/GDS/Konstant/Message count too large\n");
    if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=(f->val&0x1f)<<8;
  }

  m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_SOPP))&0x7f)<<16;

  m_i|=(k_u32)0b101111111<<23;

  r=emit_32(u_cpu2le32(m_i),m_sz_max,m,m_sz,msgs);

exit:
  return r;
}
#undef MSG
//end s_waitcnt
//==============================================================================

//==============================================================================
//s_endpgm
static k_s8 s_endpgm(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=0;

  if(i->fs[0].f!=F_INVALID){
    r=msg(msgs,_("error:machine emit:s_endpgm:%s:%d:pp(%d):no field are allowed"
                      " for this instruction\n"),i->src_pathname,i->src_l,i->l);
    if(!r) r=CMINGCNASM_ERR;goto exit;
  }

  k_u32 m_i=0;
  m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_SOPP))&0x7f)<<16;
  m_i|=(k_u32)0b101111111<<23;

  r=emit_32(u_cpu2le32(m_i),m_sz_max,m,m_sz,msgs);

exit:
  return r;
}
//end s_endpgm
//==============================================================================

static k_s8 sopp(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=0;
  switch(i->map->op_base){
  case 1:
    r=s_endpgm(i,m_sz_max,m,m_sz,msgs);
    break;
  case 12:
    r=s_waitcnt(i,m_sz_max,m,m_sz,msgs);
    break;
  default:
   r=msg(msgs,_("error:machine emit:%s:%d:pp(%d):unknown sopp opcode %d\n"),
                                 i->src_pathname,i->src_l,i->l,i->map->op_base);
   if(!r) r=CMINGCNASM_ERR;
  }
  return r;
}

//==============================================================================
//export
static k_s8 export_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
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
      r=msg(msgs,_("error:machine emit:export:%s:%d:pp(%d):%s field illegal in"
                           " this instruction\n"),i->src_pathname,i->src_l,i->l,
                                                     fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

#define MSG(x,y) msg(msgs,_(x ":machine emit:export:%s:%d:pp(%d):" y),\
i->src_pathname,i->src_l,i->l)
static k_s8 export(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=export_fs_chk(i,msgs);
  if(r) goto exit;

  k_u64 m_i=0;

  struct i_f *f=f_get(i,F_EN);
  if(f){
    if(f->val>0xf){
      r=MSG("error","enable mask too large\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
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

  m_i|=(k_u64)0b111110<<26;

  k_u8 vsrc0_set=0;
  f=f_get(i,F_VSRC0);
  if(f){
    m_i|=(k_u64)(f->val&0xff)<<32;
    vsrc0_set=1;
  }
  
  k_u8 vsrc1_set=0;
  f=f_get(i,F_VSRC1);
  if(f){
    m_i|=(k_u64)(f->val&0xff)<<40;
    vsrc1_set=1;
  }
  
  k_u8 vsrc2_set=0;
  f=f_get(i,F_VSRC2);
  if(f){
    m_i|=(k_u64)(f->val&0xff)<<48;
    vsrc2_set=1;
  }

  k_u8 vsrc3_set=0;
  f=f_get(i,F_VSRC3);
  if(f){
    m_i|=(k_u64)(f->val&0xff)<<56;
    vsrc3_set=1;
  }

  //----------------------------------------------------------------------------
  //start instruction scale checks (warnings and errors)
  //----------------------------------------------------------------------------
  //useless to enable vsrc2 and vsrc3 when compr=1
  if(m_i>>10&1&&m_i&0x3){
    r=MSG("warning","compr is 1 then useless to enable vsrc2 and/or vsrc3\n");
    if(r) goto exit;
  }
  if(m_i&1&&!vsrc0_set){
    r=MSG("warning","vsrc0 export is enabled without vsrc0 field\n");
    if(r) goto exit;
  }
  if(!m_i&1&&vsrc0_set){
    r=MSG("warning","vsrc0 export is disabled but vsrc0 field is defined\n");
    if(r) goto exit;
  }
  if(m_i&1&&!vsrc1_set){
    r=MSG("warning","vsrc1 export is enabled without vsrc1 field\n");
    if(r) goto exit;
  }
  if(!m_i&1&&vsrc1_set){
    r=MSG("warning","vsrc1 export is disabled but vsrc1 field is defined\n");
    if(r) goto exit;
  }
  if(m_i&1&&!vsrc2_set){
    r=MSG("warning","vsrc2 export is enabled without vsrc2 field\n");
    if(r) goto exit;
  }
  if(!m_i&1&&vsrc2_set){
    r=MSG("warning","vsrc2 export is disabled but vsrc2 field is defined\n");
    if(r) goto exit;
  }
  if(m_i&1&&!vsrc3_set){
    r=MSG("warning","vsrc3 export is enabled without vsrc3 field\n");
    if(r) goto exit;
  }
  if(!m_i&1&&vsrc3_set){
    r=MSG("warning","vsrc3 export is disabled but vsrc3 field is defined\n");
    if(r) goto exit;
  }
  //----------------------------------------------------------------------------

  r=emit_64(u_cpu2le64(m_i),m_sz_max,m,m_sz,msgs);

exit:
  return r;
}
#undef MSG
//end export
//==============================================================================

//==============================================================================
//vop3a
#define MSG(x,...) msg(msgs,_("error:machine emit:%s(vop3a):%s:%d:pp(%d):" x),\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static k_s8 vop3a_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
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
      r=MSG("%s field illegal in this instruction\n",fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

static k_s8 vop3a(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=vop3a_fs_chk(i,msgs);
  if(r) goto exit;

  k_u64 m_i=0;

  struct i_f *f=f_get(i,F_VDST);
  if(f) m_i|=f->val&0xff;

  f=f_get(i,F_ABS);
  if(f){
    if(f->val>3){
      r=MSG("vsrc index for absolute value above 3\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=(f->val&3)<<8;
  }

  f=f_get(i,F_CLAMP);
  if(f) m_i|=(f->val&1)<<11;

  m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP3A))&0x1ff)<<17;

  m_i|=(k_u64)0b110100<<26;

  f=f_get(i,F_SRC0);
  if(f) m_i|=(k_u64)(f->val&0x1ff)<<32;

  f=f_get(i,F_SRC1);
  if(f) m_i|=(k_u64)(f->val&0x1ff)<<41;

  f=f_get(i,F_SRC2);
  if(f) m_i|=(k_u64)(f->val&0x1ff)<<50;

  f=f_get(i,F_OMOD);
  if(f){
    if(f->val>3){
      r=MSG("output modifier above 3\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=(k_u64)(f->val&3)<<59;
  }

  f=f_get(i,F_NEG);
  if(f){
    if(f->val>3){
      r=MSG("vsrc index for negation value above 3\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=(k_u64)(f->val&3)<<61;
  }

  r=emit_64(u_cpu2le64(m_i),m_sz_max,m,m_sz,msgs);
  
exit:
  return r;
}
#undef MSG
//end of vop3a
//==============================================================================

//==============================================================================
//vop1
#define MSG(x,...) msg(msgs,_("error:machine emit:%s(vop1):%s:%d:pp(%d):" x),\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static k_s8 vop1_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
    if(i->fs[i_f].f==F_INVALID) break;

    switch(i->fs[i_f].f){
    case F_SRC0:
    case F_VDST:
      break;
    default:
      r=MSG("%s field illegal in this instruction\n",fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

static k_s8 vop1(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=vop1_fs_chk(i,msgs);
  if(r) goto exit;

  k_u32 m_i=0;

  struct i_f *f=f_get(i,F_SRC0);
  if(f){
    if(f->val>0x1ff){
      r=MSG("src0 value too large\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=f->val&0x1ff;
  }

  m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP1))&0xff)<<9;

  f=f_get(i,F_VDST);
  if(f) m_i|=(f->val&0xff)<<17;

  m_i|=(k_u32)0b0111111<<25;

  r=emit_32(u_cpu2le32(m_i),m_sz_max,m,m_sz,msgs);
  
exit:
  return r;
}
#undef MSG
//end of vop1
//==============================================================================

//==============================================================================
//vop2
#define MSG(x,...) msg(msgs,_("error:machine emit:%s(vop2):%s:%d:pp(%d):" x),\
i->map->mnemonic,i->src_pathname,i->src_l,i->l,##__VA_ARGS__)
static k_s8 vop2_fs_chk(struct i *i,struct msgs_ctx *msgs)
{
  k_s8 r=0;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
    if(i->fs[i_f].f==F_INVALID) break;

    switch(i->fs[i_f].f){
    case F_SRC0:
    case F_VSRC1:
    case F_VDST:
      break;
    default:
      r=MSG("%s field illegal in this instruction\n",fs_mnemonic[i->fs[i_f].f]);
      if(!r) r=CMINGCNASM_ERR;
    }

    if(r) break;
  }
  return r;
}

static k_s8 vop2(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=vop2_fs_chk(i,msgs);
  if(r) goto exit;

  k_u32 m_i=0;

  struct i_f *f=f_get(i,F_SRC0);
  if(f){
    if(f->val>0x1ff){
      r=MSG("src0 value too large\n");
      if(!r) r=CMINGCNASM_ERR;goto exit;
    }
    m_i|=f->val&0x1ff;
  }

  f=f_get(i,F_VSRC1);
  if(f) m_i|=(f->val&0xff)<<9;

  f=f_get(i,F_VDST);
  if(f) m_i|=(f->val&0xff)<<17;

  m_i|=((i->map->op_base+fmt_op_offset(i->map->fmts,FMT_VOP2))&0x3f)<<25;

  m_i|=(k_u32)0b0<<31;

  r=emit_32(u_cpu2le32(m_i),m_sz_max,m,m_sz,msgs);
  
exit:
  return r;
}
#undef MSG
//end of vop2
//==============================================================================

//==============================================================================
//vop1|vop3a
#define VOP1  0
#define VOP3A 1
static k_s8 vop1_vop3a_select(struct i *i)
{
  k_s8 r=VOP1;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
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
  }
  return r;
}

static k_s8 vop1_vop3a(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=vop1_vop3a_select(i);
  if(r<0) goto exit;

  switch(r){
  case VOP1:
    r=vop1(i,m_sz_max,m,m_sz,msgs);
    break;
  case VOP3A:
    r=vop3a(i,m_sz_max,m,m_sz,msgs);
    break;
  }   
exit:
  return r;
}
//end of vop1|vop3a
//==============================================================================
//
//==============================================================================
//vop2|vop3a
#define VOP2  0
static k_s8 vop2_vop3a_select(struct i *i)
{
  k_s8 r=VOP2;
  for(k_u8 i_f=0;i_f<FS_MAX;++i_f){
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
  }
  return r;
}

static k_s8 vop2_vop3a(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=vop2_vop3a_select(i);
  if(r<0) goto exit;

  switch(r){
  case VOP2:
    r=vop2(i,m_sz_max,m,m_sz,msgs);
    break;
  case VOP3A:
    r=vop3a(i,m_sz_max,m,m_sz,msgs);
    break;
  }   
exit:
  return r;
}
//end of vop1|vop3a
//==============================================================================

static k_s8 i_encode(struct i *i,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=0;
  switch(i->map->fmts){
  case BIT(FMT_MUBUF):
    r=mubuf(i,m_sz_max,m,m_sz,msgs);
    break;
  case BIT(FMT_SOPP):
    r=sopp(i,m_sz_max,m,m_sz,msgs);
    break;
  case BIT(FMT_EXP):
    r=export(i,m_sz_max,m,m_sz,msgs);
    break;
  case BIT(FMT_VOP1)|BIT(FMT_VOP3A):
    r=vop1_vop3a(i,m_sz_max,m,m_sz,msgs);
    break;
  case BIT(FMT_VOP2)|BIT(FMT_VOP3A):
    r=vop2_vop3a(i,m_sz_max,m,m_sz,msgs);
    break;
  default:
    r=msg(msgs,_("error:machine emit:%s:%d:pp(%d):instruction encoder not"
             " supported 0x%08x\n"),i->src_pathname,i->src_l,i->l,i->map->fmts);
    if(!r) r=CMINGCNASM_ERR;
  }
  return r;
}

k_s8 m_emit(struct i *is,k_s32 is_last,k_s32 m_sz_max,void **m,k_s32 *m_sz,
                                                          struct msgs_ctx *msgs)
{
  k_s8 r=0;

  *m=0;
  *m_sz=0;
  for(k_s32 i=0;i<=is_last;++i){
    r=i_encode(&is[i],m_sz_max,m,m_sz,msgs);
    if(r!=0) break;
  }
 
  if(r!=0) unmap(*m,*m_sz,msgs);
  return r;
}
