//x86-64 stack

#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/sched.h>
#include <ulinux/mmap.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u8 ulinux_u8
#define exit(a) ulinux_sysc(exit,1,a)
#define mmap(a,b,c,d,e,f) ulinux_sysc(mmap,6,a,b,c,d,e,f)
#define RD ULINUX_PROT_READ
#define WR ULINUX_PROT_WRITE
#define PRIVATE ULINUX_MAP_PRIVATE
#define ANONYMOUS ULINUX_MAP_ANONYMOUS
#define STACK ULINUX_MAP_STACK
#define clone(a,b,c,d,e) ulinux_sysc(clone,5,a,b,c,d,e)
#define THREAD ULINUX_CLONE_THREAD
#define SIGHAND ULINUX_CLONE_SIGHAND
#define VM ULINUX_CLONE_VM
#define FILES ULINUX_CLONE_FILES
#define FS ULINUX_CLONE_FS
#define IO ULINUX_CLONE_IO
#define SYSVSEM ULINUX_CLONE_SYSVSEM
//------------------------------------------------------------------------------

#define STACK_SZ 8*1024*1024

//don't let gcc optimize the thread entry function
static void __attribute__((noreturn,noinline,noclone)) thread_entry(void)
{
	while(1);
	exit(0);
}

void _start(void)
{
  l r=mmap(0,STACK_SZ,RD|WR,PRIVATE|ANONYMOUS|STACK,0,0);

  l stack_bottom=r;
  l stack_top_unaligned=stack_bottom+STACK_SZ;
  l stack_top_aligned=stack_top_unaligned & (~0x1f);//32 bytes aligned

  r=clone(THREAD|SIGHAND|VM|FILES|FS|IO|SYSVSEM,stack_top_aligned,0,0,0);
  while(1);
  exit(0);
}
