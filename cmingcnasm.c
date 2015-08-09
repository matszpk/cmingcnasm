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
#include <ulinux/compiler_misc.h>
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
#include <cmingcnasm.h>
/*----------------------------------------------------------------------------*/

#ifndef QUIET
u8 *g_dprintf_buf;
#define DPRINTF_BUF_SZ 2048
#define PERR(f,...) ulinux_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,\
(u8*)f,## __VA_ARGS__)
#define PERRC(s) ulinux_sysc(write,3,2,s,sizeof(s)-1)
#define POUTC(s) ulinux_sysc(write,3,1,s,sizeof(s)-1)
#endif

#define SRC_PATHNAME_DEFAULT "source.sgcn"
#define M_PATHNAME_DEFAULT "machine.bgcn"

static u8 *src_pathname;
static si src_fd;
static u8 *src;
static u64 src_sz;

static u8 *m_pathname;
static si m_fd;
static u8 *m;
static u64 m_sz;

static void args_parse(sl argc,u8 **argv_envp)
{
	u8 dash_options_enabled;
	u8 src_pathname_missing;
	u8 m_pathname_missing;
	sl arg;

	dash_options_enabled=1;
	src_pathname_missing=1;
	m_pathname_missing=1;
	arg=1;/*skip program pathname*/

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
"cmingcnasm [OPTIONS] [--] [SOURCE FILE] [MACHINE FILE]\n"
"  OPTIONS:\n"
"    -h: help, this message\n"
"  [--]: to allow OPTIONS (i.e. -h) to begin SOURCE or MACHINE pathname\n"
"  SOURCE FILE: input file, default='" SRC_PATHNAME_DEFAULT  "'\n"
"  MACHINE FILE: output file, default='" M_PATHNAME_DEFAULT "'\n");
				exit(0);
			}
		}

		if(src_pathname_missing){
			src_pathname=&argv_envp[arg][0];
			src_pathname_missing=0;
			++arg;
			continue;
		}

		if(m_pathname_missing){
			m_pathname=&argv_envp[arg][0];
			m_pathname_missing=0;
			++arg;
			continue;
		}
	}
}

static void src_mmap(void)
{
	sl r;
	struct stat src_stat;

	loop{
		r=open(src_pathname,O_RDONLY,0);
		if(r!=EINTR) break;
	}
	if(ISERR(r)){
		PERR("fatal(%ld):unable to open source file\n",r);
		exit(-1);
	}
	src_fd=(si)r;

	r=fstat(src_fd,&src_stat);
	if(ISERR(r)){
		PERR("fatal(%ld):unable to stat source file\n",r);
		exit(-1);
	}

	src_sz=(u64)src_stat.sz;/*broken signed off_t...*/
	if(!src_sz){
		PERR("WARNING:source file is empty, exiting with no error\n");
		exit(-1);
	}

	r=mmap(src_sz,PROT_READ,MAP_PRIVATE,src_fd);
	if(ISERR(r)){
		PERR("fatal(%ld):unable to mmap source file\n",r);
		exit(-1);
	}
	src=(u8*)r;
}

static void m_save(void)
{
	sl r;
	u64 bytes_written;

	loop{
		r=open(m_pathname,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR
							|S_IRGRP|S_IROTH);
		if(r!=-EINTR) break;
	}
	if(ISERR(r)){
		PERR("fatal(%ld):unable to open machine file\n",r);
		exit(-1);
	}
	m_fd=(si)r;

	bytes_written=0;
	loop{
		loop{
			r=write(m_fd,m+bytes_written,m_sz-bytes_written);
			if(r!=-EINTR) break;
		}
		if(ISERR(r)){
      			PERR("fatal(%ld):error writing machine file\n",r);
			exit(-1);
		}
		bytes_written+=(u64)r;
		if(bytes_written==m_sz) break;
	}
}

static void globals_init()
{
	src_pathname=(u8*)SRC_PATHNAME_DEFAULT;
	src_fd=-1;
	src=0;
	src_sz=0;

	m_pathname=(u8*)M_PATHNAME_DEFAULT;
	m_fd=-1;
	m=0;
	m_sz=0;

	cmingcnasm_static_init();
}

/******************************************************************************/
void ulinux_start(sl argc,u8 **argv_envp)
{
	u8 *msgs;
  	u64 msgs_sz;
	s8 r0;
#ifndef QUIET 
	static u8 dprintf_buf[DPRINTF_BUF_SZ];
	g_dprintf_buf=dprintf_buf;
#endif
	globals_init();
	args_parse(argc,argv_envp);
	src_mmap();

	r0=cmingcnasm_asm(	src,
				src_sz,
				src_pathname,
				&m,
				&m_sz,
				&msgs,
				&msgs_sz);

	if(msgs_sz){
		sl r1;
		loop{
			r1=write(2,msgs,msgs_sz);
			if(r1!=-EINTR&&r1!=-EAGAIN) break;
		}
  	}
	if(r0==CMINGCNASM_ERR){
		PERR("fatal(%d)\n",r0);
		exit(-1);
	}
	m_save();
	exit(0);
}
