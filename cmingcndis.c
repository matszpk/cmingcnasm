/*******************************************************************************
this code is protected by the GNU affero GPLv3
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
#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/stat.h>
#include <ulinux/mmap.h>

#include <ulinux/utils/mem.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>

#include "ulinux-namespace.h"
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
used libs
------------------------------------------------------------------------------*/
#include <cmingcndis.h>
/*----------------------------------------------------------------------------*/

#ifndef QUIET
u8 *g_dprintf_buf;
#define DPRINTF_BUF_SZ 2048
#define PERR(f,...) ulinux_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,(u8*)f,\
##__VA_ARGS__)
#define PERRC(s) ulinux_sysc(write,3,2,s,sizeof(s)-1)
#define POUTC(s) ulinux_sysc(write,3,1,s,sizeof(s)-1)
#endif

#define M_PATHNAME_DEFAULT "machine.bgcn"
#define SRC_PATHNAME_DEFAULT "source.sgcn"

static u8 *m_pathname=(u8*)M_PATHNAME_DEFAULT;
static si m_fd=-1;
static void *m=0;
static s32 m_sz=0;

static u8 *src_pathname=(u8*)SRC_PATHNAME_DEFAULT;
static si src_fd=-1;
static u8 *src=0;
static s32 src_sz=0;
static s32 src_sz_max=1024*1024;/*default to 1MiB*/

static void args_parse(sl argc,u8 **argv_envp)
{
	u8 dash_options_enabled=1;
	u8 src_pathname_missing=1;
	u8 m_pathname_missing=1;
	sl arg=1;/*skip program pathname*/

	loop{
		if(arg+1>argc) break;

		if(dash_options_enabled){
			if(strncmp("--",argv_envp[arg],2)==0){
				dash_options_enabled=0;
				++arg;
				continue;
			}
			if(strncmp("-h",argv_envp[arg],2)==0){
				POUTC(
"cmingcndis [OPTIONS] [--] [MACHINE FILE] [SOURCE FILE]\n"
"  OPTIONS:\n"
"    -h: help, this message\n"
"  [--]: to allow OPTIONS (i.e. -h) to begin MACHINE or SOURCE pathname\n"
"  MACHINE FILE: input file, default='" M_PATHNAME_DEFAULT "'\n"
"  SOURCE FILE: output file, default='" SRC_PATHNAME_DEFAULT "'\n");
				exit(0);
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
	sl r;
	struct stat m_stat;
	loop{
		r=open(m_pathname,O_RDONLY,0);
		if(r!=-EINTR) break;
	}
	if(ISERR(r)){
		PERR("fatal(%ld):unable to open machine file\n",r);
		exit(-1);
	}
	m_fd=(si)r;

	r=fstat(m_fd,&m_stat);
	if(ISERR(r)){
		PERR("fatal(%ld):unable to stat machine file\n",r);
		exit(-1);
	}
	m_sz=(s32)m_stat.sz;

	if(!m_sz){
		PERR("WARNING:machine file is empty, exiting with no error\n");
		exit(0);
	}
  
	r=mmap(m_sz,PROT_READ,MAP_PRIVATE,m_fd);
	if(ISERR(r)){
		PERR("fatal(%ld):unable to mmap machine file\n",r);
		exit(-1);
	}
	m=(u8*)r;
}

static void src_save(void)
{
	sl r;
	s32 bytes_written;

	loop{
		r=open(src_pathname,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR
							|S_IRGRP|S_IROTH);
		if(r!=-EINTR) break;
	}
	if(ISERR(r)){
		PERR("fatal(%ld):unable to open source file\n",r);
		exit(-1);
	}
	src_fd=(si)r;

	bytes_written=0;
	loop{
		loop{
			r=write(src_fd,src+bytes_written,src_sz-bytes_written);
			if(r!=-EINTR) break;
		}
		if(ISERR(r)){
			PERR("fatal(%ld):error writing source file\n",r);
			exit(-1);
		}
		bytes_written+=(s32)r;
		if(bytes_written==src_sz) break;
	}
}

/******************************************************************************/
void ulinux_start(sl argc,u8 **argv_envp)
{
	u8 *msgs;
	s32 msgs_sz;
	s8 r0;
#ifndef QUIET 
	static u8 dprintf_buf[DPRINTF_BUF_SZ];
	g_dprintf_buf=dprintf_buf;
#endif
	args_parse(argc,argv_envp);
	m_mmap();

	r0=cmingcndis_dis(	m,
				m_sz,
				src_sz_max,
				&src,
				&src_sz,
				10*1024,/*max 10kiB of messages*/
				&msgs,
				&msgs_sz);

	if(msgs_sz){
		sl r1;
		loop{
			r1=write(2,msgs,msgs_sz);
			if(r1!=-EINTR&&r1!=-EAGAIN) break;
  		}
	}
	if(r0==CMINGCNDIS_ERR){
		PERR("fatal(%d)\n",r0);
		exit(-1);
	}else if(r0==CMINGCN_MSGS_ERR){
		PERR("fatal(%d):something went wrong with the message system\n",
									r0);
		exit(-1);
	}

	src_save();
	exit(0);
}
