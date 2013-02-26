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
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//private
//------------------------------------------------------------------------------
#include "msgs.h"
#include "cmingcnasm.h"
//------------------------------------------------------------------------------

static k_u8 grow(struct msgs_ctx *msgs,k_s32 len)
{
  k_s8 r=0;
  k_l addr;

  if(!*msgs->sz){//first allocation, then mmapping
    addr=sysc(mmap,6,0,len,K_PROT_READ|K_PROT_WRITE,
                                            K_MAP_PRIVATE|K_MAP_ANONYMOUS,-1,0);
    if(K_ISERR(addr)){
      r=CMINGCNASM_MSGS_ERR;goto exit;
    }
  }else{//remapping
    if(*msgs->sz+len>msgs->sz_max){
      r=CMINGCNASM_MSGS_ERR;goto exit;     
    }

    addr=sysc(mremap,5,*msgs->msgs,*msgs->sz,*msgs->sz+len,K_MREMAP_MAYMOVE,0);
    if(K_ISERR(addr)){
      r=CMINGCNASM_MSGS_ERR;goto exit;
    }
  }

  *msgs->msgs=(void*)addr;
  *msgs->sz=*msgs->sz+len;
exit:
  return r; 
}

k_s8 msg(struct msgs_ctx *msgs,k_u8 *fmt,...)
{
  va_list args;
  if(!msgs->msgs||!msgs->sz) return 0;

  k_s32 r=0;
  va_start(args,fmt);
  k_l len=u_a_vsnprintf(0,0,fmt,args);//compute needed space
  va_end(args);
  if(len<0) r=CMINGCNASM_MSGS_ERR;
  else{
    k_s32 old_sz=*msgs->sz;
    r=grow(msgs,len+1);
    if(!r){
      va_start(args,fmt);
      k_s32 shift=old_sz?1:0;
      len=u_a_vsnprintf(*msgs->msgs+old_sz-shift,len+1,fmt,args);
      va_end(args);
      if(len<0) r=CMINGCNASM_MSGS_ERR;
    }
  }
  return r;
}
