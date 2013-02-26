//x86-64 stack

#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/sched.h>
#include <ulinux/mmap.h>

#define STACK_SZ 8*1024*1024

//don't let gcc optimize the thread entry function
static void __attribute__((noreturn,noinline,noclone)) thread_entry(void)
{
	while(1);
	sysc(exit,1,0);
}

void _start(void)
{
  k_l r=sysc(mmap,6,0,STACK_SZ,K_PROT_READ|K_PROT_WRITE,K_MAP_PRIVATE
                      |K_MAP_ANONYMOUS|K_MAP_STACK,0,0);

  k_l stack_bottom=r;
  k_l stack_top_unaligned=stack_bottom+STACK_SZ;
  k_l stack_top_aligned=stack_top_unaligned & (~0x1f);//32 bytes aligned

  r=sysc(clone,5,K_CLONE_THREAD|K_CLONE_SIGHAND|K_CLONE_VM|K_CLONE_FILES
                 |K_CLONE_FS|K_CLONE_IO|K_CLONE_SYSVSEM,stack_top_aligned,0,0,
                 0);
  while(1);
  sysc(exit_group,1,0);
}
