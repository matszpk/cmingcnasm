//******************************************************************************
//this code is protected by the GNU affero GPLv3
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
#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/stat.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//used libs
//------------------------------------------------------------------------------
#include <cmingcndis.h>
//------------------------------------------------------------------------------

#ifndef QUIET
k_u8 *g_dprintf_buf;
#define DPRINTF_BUF_SZ 2048
#define PERR(f,...) u_a_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
##__VA_ARGS__)
#define PERRC(s) sysc(write,3,2,s,sizeof(s)-1)
#define POUTC(s) sysc(write,3,1,s,sizeof(s)-1)
#endif

#define _(x) ((k_u8*)x)

#define M_PATHNAME_DEFAULT "machine.bgcn"
#define SRC_PATHNAME_DEFAULT "source.sgcn"

static k_u8 *m_pathname=_(M_PATHNAME_DEFAULT);
static k_i m_fd=-1;
static void *m=0;
static k_s32 m_sz=0;

static k_u8 *src_pathname=_(SRC_PATHNAME_DEFAULT);
static k_i src_fd=-1;
static k_u8 *src=0;
static k_s32 src_sz=0;
static k_s32 src_sz_max=1024*1024;//default to 1MB
static void args_parse(k_i argc,k_u8 **argv_envp)
{
  k_u8 dash_options_enabled=1;
  k_u8 src_pathname_missing=1;
  k_u8 m_pathname_missing=1;

  k_i arg=1;//skip program pathname

  while(arg+1<=argc){
    if(dash_options_enabled){
      if(u_a_strncmp(_("--"),argv_envp[arg],2)==0){
        dash_options_enabled=0;
        ++arg;
        continue;
      }
      if(u_a_strncmp(_("-h"),argv_envp[arg],2)==0){
        POUTC(
"cmingcndis [OPTIONS] [--] [MACHINE FILE] [SOURCE FILE]\n"
"  OPTIONS:\n"
"    -h: help, this message\n"
"  [--]: to allow OPTIONS (i.e. -h) to start MACHINE or SOURCE pathname\n"
"  MACHINE FILE: input file, default='" M_PATHNAME_DEFAULT "'\n"
"  SOURCE FILE: output file, default='" SRC_PATHNAME_DEFAULT "'\n"
        );
        sysc(exit_group,1,0);
      }
    }

    if(m_pathname_missing){
      m_pathname=&argv_envp[arg][0];
      m_pathname_missing=0;
      ++arg;
      continue;
    }

    if(src_pathname_missing){
      src_pathname=&argv_envp[arg][0];
      src_pathname_missing=0;
      ++arg;
      continue;
    }
  }
}

static void m_mmap(void)
{
  k_l r;
  do r=sysc(open,3,m_pathname,K_O_RDONLY,0);while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to open machine file\n",r);
    sysc(exit_group,1,-1);
  }
  m_fd=(k_i)r;

  struct k_stat m_stat;
  r=sysc(fstat,2,m_fd,&m_stat);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to stat machine file\n",r);
    sysc(exit_group,1,-1);
  }
  m_sz=(k_s32)m_stat.sz;

  if(!m_sz){
    PERR("WARNING:machine file is empty, exiting with no error\n");
    sysc(exit_group,1,0);
  }
  
  r=sysc(mmap,6,0,m_sz,K_PROT_READ,K_MAP_PRIVATE,m_fd,0);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to mmap machine file\n",r);
    sysc(exit_group,1,-1);
  }
  m=(void*)r;
}

static void src_save(void)
{
  k_l r;
  do
    r=sysc(open,3,src_pathname,K_O_CREAT|K_O_TRUNC|K_O_WRONLY,K_S_IRUSR
                                                |K_S_IWUSR|K_S_IRGRP|K_S_IROTH);
  while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to open source file\n",r);
    sysc(exit_group,1,-1);
  }
  src_fd=(k_i)r;

  k_s32 bytes_written=0;
  do{
    do
      r=sysc(write,3,src_fd,src+bytes_written,src_sz-bytes_written);
    while(r==-K_EINTR);
    if(K_ISERR(r)){
      PERR("fatal(%ld):error writing source file\n",r);
      sysc(exit_group,1,-1);
    }
    bytes_written+=(k_s32)r;
  }while(bytes_written!=src_sz);
}

//******************************************************************************
void start(k_i argc, k_u8 **argv_envp)
{
#ifndef QUIET 
  static k_u8 dprintf_buf[DPRINTF_BUF_SZ];
  g_dprintf_buf=dprintf_buf;
#endif
  args_parse(argc,argv_envp);
  m_mmap();

  k_u8 *msgs;
  k_s32 msgs_sz;

  k_s8 r0=cmingcndis_dis(m,
                         m_sz,
                         src_sz_max,
                         &src,
                         &src_sz,
                         10*1024,//max 10kB of messages
                         &msgs,
                         &msgs_sz);

  if(msgs_sz){
    k_l r1;
    do r1=sysc(write,3,2,msgs,msgs_sz); while(r1==-K_EINTR||r1==-K_EAGAIN);
  }
  if(r0==CMINGCNDIS_ERR){
    PERR("fatal(%d)\n",r0);
    sysc(exit_group,1,-1);
  }else if(r0==CMINGCNDIS_MSGS_ERR){
    PERR("fatal(%d):something went wrong with the message system\n",r0);
    sysc(exit_group,1,-1);
  }

  src_save();
  sysc(exit_group,1,0);
}
