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

#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/mem.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//used libs
//------------------------------------------------------------------------------
#include <cmingcnasm.h>
//------------------------------------------------------------------------------

#ifndef QUIET
k_u8 *g_dprintf_buf;
#define DPRINTF_BUF_SZ 2048
#define PERR(f,...) u_a_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,\
(k_u8*)f,## __VA_ARGS__)
#define POUT(f,...) u_a_dprintf(1,g_dprintf_buf,DPRINTF_BUF_SZ,\
(k_u8*)f,## __VA_ARGS__)
#define PERRC(s) sysc(write,3,2,s,sizeof(s)-1)
#define POUTC(s) sysc(write,3,1,s,sizeof(s)-1)
#endif

#define _(x) ((k_u8*)x)

static k_u8 *source_filename=_("source.sgcn");
static k_i source_fd=-1;
static void *source=0;
static k_s32 source_sz=0;

static k_u8 *machine_filename=_("machine.bgcn");
static k_i machine_fd=-1;
static void *machine=0;
static k_s32 machine_sz=0;
static k_s32 machine_sz_max=1024*1024;//default to 1MB

static void args_parse(k_i argc,k_u8 **argv_envp)
{
  k_u8 dash_options_enabled=1;
  k_u8 source_filename_missing=1;
  k_u8 machine_filename_missing=1;

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
"cmingcnasm [OPTIONS] [--] [SOURCE FILE] [MACHINE FILE]\n"
"  OPTIONS:\n"
"    -h: help, this message\n"
"  [--]: to allow OPTIONS (i.e. -h) to start SOURCE or MACHINE file name\n"
"  SOURCE FILE: input file, default='source.sgcn'\n"
"  MACHINE FILE: output file, default='machine.bgcn'\n"
        );
        sysc(exit_group,1,0);
      }
    }

    if(source_filename_missing){
      source_filename=&argv_envp[arg][0];
      source_filename_missing=0;
      ++arg;
      continue;
    }

    if(machine_filename_missing){
      machine_filename=&argv_envp[arg][0];
      machine_filename_missing=0;
      ++arg;
      continue;
    }
  }
}

static void source_mmap(void)
{
  k_l r;
  do r=sysc(open,3,source_filename,K_O_RDONLY,0);while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to open source file\n",r);
    sysc(exit_group,1,-1);
  }
  source_fd=(k_i)r;

  struct k_stat source_stat;
  r=sysc(fstat,2,source_fd,&source_stat);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to stat source file\n",r);
    sysc(exit_group,1,-1);
  }
  source_sz=(k_s32)source_stat.sz;

  if(!source_sz){
    PERR("WARNING:source file is empty, exiting with no error\n");
    sysc(exit_group,1,0);
  }
  
  r=sysc(mmap,6,0,source_sz,K_PROT_READ,K_MAP_PRIVATE,source_fd,0);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to mmap source file\n",r);
    sysc(exit_group,1,-1);
  }
  source=(void*)r;
}

static void machine_save(void)
{
  k_l r;
  do
    r=sysc(open,3,machine_filename,K_O_CREAT|K_O_TRUNC|K_O_WRONLY,K_S_IRUSR
                                                |K_S_IWUSR|K_S_IRGRP|K_S_IROTH);
  while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("fatal(%ld):unable to open machine file\n",r);
    sysc(exit_group,1,-1);
  }
  machine_fd=(k_i)r;

  k_s32 bytes_written=0;
  do{
    do
      r=sysc(write,3,machine_fd,machine+bytes_written,machine_sz-bytes_written);
    while(r==-K_EINTR);
    if(K_ISERR(r)){
      PERR("fatal(%ld):error writing machine file\n",r);
      sysc(exit_group,1,-1);
    }
    bytes_written+=(k_s32)r;
  }while(bytes_written!=machine_sz);
}

//******************************************************************************
void start(k_i argc,k_u8 **argv_envp)
{
#ifndef QUIET 
  static k_u8 dprintf_buf[DPRINTF_BUF_SZ];
  g_dprintf_buf=dprintf_buf;
#endif
  args_parse(argc,argv_envp);
  source_mmap();

  k_u8 *msgs;
  k_s32 msgs_sz;

  k_s8 r0=cmingcnasm_asm(source,
                         source_sz,
                         source_filename,
                         machine_sz_max,
                         &machine,
                         &machine_sz,
                         10*1024,//max 10kB of messages
                         &msgs,
                         &msgs_sz);

  if(msgs_sz){
    k_l r1;
    do r1=sysc(write,3,2,msgs,msgs_sz); while(r1==-K_EINTR||r1==-K_EAGAIN);
  }
  if(r0==CMINGCNASM_ERR){
    PERR("fatal(%d)\n",r0);
    sysc(exit_group,1,-1);
  }else if(r0==CMINGCNASM_MSGS_ERR){
    PERR("fatal(%d):something went wrong with the message system\n",r0);
    sysc(exit_group,1,-1);
  }

  machine_save();
  sysc(exit_group,1,0);
}
