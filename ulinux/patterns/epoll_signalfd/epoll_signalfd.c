#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/utils/mem.h>
#include <ulinux/epoll.h>
#include <ulinux/file.h>
#include <ulinux/error.h>
#include <ulinux/signal/signal.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u64 ulinux_u64
#define exit(code) ulinux_sysc(exit_group,1,code)
#define rt_sigprocmask(a,b,c,d) ulinux_sysc(rt_sigprocmask,4,a,b,c,d)
#define SIG_BLOCK ULINUX_SIG_BLOCK
#define SIGUSR1 ULINUX_SIGUSR1
#define SIGUSR2 ULINUX_SIGUSR2
#define ISERR ULINUX_ISERR
#define signalfd4(a,b,c,d) ulinux_sysc(signalfd4,4,a,b,c,d)
#define SFD_NONBLOCK ULINUX_SFD_NONBLOCK
#define epoll_create1(a) ulinux_sysc(epoll_create1,1,a)
#define epoll_event ulinux_epoll_event
#define memset ulinux_memset
#define EPOLLET ULINUX_EPOLLET
#define EPOLLIN ULINUX_EPOLLIN
#define epoll_ctl(a,b,c,d) ulinux_sysc(epoll_ctl,4,a,b,c,d)
#define EPOLL_CTL_ADD ULINUX_EPOLL_CTL_ADD
#define epoll_wait(a,b,c,d) ulinux_sysc(epoll_wait,4,a,b,c,d)
#define signalfd_siginfo ulinux_signalfd_siginfo
#define read(a,b,c) ulinux_sysc(read,3,a,b,c)
//------------------------------------------------------------------------------

#define cs_n(str) (sizeof(str)-1)
#define POUTC(str) {l rl;do{rl=ulinux_sysc(write,3,1,str,cs_n(str));}\
while(rl==-ULINUX_EINTR||rl==-ULINUX_EAGAIN);}
#define SIGBIT(sig) (1<<(sig-1))
#define EPOLL_EVENTS_N 10

//don't let gcc optimize the thread entry function
static void __attribute__((noreturn,noinline,noclone)) thread_entry(void)
{
	while(1);
	exit(0);
}

void _start(void)
{
  //synchronous treatement of signals with signalfd
  u64 mask=(~0);//don't forget, you cannot even touch SIGKILL, neither SIGSTOP
  l r=rt_sigprocmask(SIG_BLOCK,&mask,0,sizeof(mask));
  if(ISERR(r)) exit(-1);

  mask=SIGBIT(SIGUSR1)|SIGBIT(SIGUSR2);
  i sigs_fd=(i)signalfd4(-1,&mask,sizeof(mask),SFD_NONBLOCK);
  if(ISERR(sigs_fd)) exit(-2);

  i epfd=(i)epoll_create1(0);
  if(ISERR(epfd)) exit(-3);

  struct epoll_event evts[EPOLL_EVENTS_N];
  memset(evts,0,sizeof(struct epoll_event));
  evts[0].events=EPOLLET|EPOLLIN;
  evts[0].data.fd=sigs_fd;
  r=epoll_ctl(epfd,EPOLL_CTL_ADD,sigs_fd,&evts[0]);
  if(ISERR(r)) exit(-4);

  while(1){
    do{
      memset(evts,0,sizeof(evts));
      r=epoll_wait(epfd,evts,EPOLL_EVENTS_N,-1);
    }while(r==-EINTR);
    if(ISERR(r)) exit(-6);

    for(l j=0;j<r;++j)
      if(evts[j].data.fd==sigs_fd){
        if(evts[j].events&EPOLLIN){
          struct signalfd_siginfo info;

          while(1){
            do{
              memset(&info,0,sizeof(info));
              r=read(sigs_fd,&info,sizeof(info));
            }while(r==-EINTR);
            if(r!=-EAGAIN&&((ISERR(r)||(r>0&&r!=sizeof(info))))) exit(-7);
            if(r==0||r==-EAGAIN) break;

            switch(info.ssi_signo){
            case SIGUSR1: POUTC("SIGUSR1\n"); break;
            case SIGUSR2: POUTC("SIGUSR2\n"); break;
            }
          }
        }else exit(-8);
      }
  }
}
