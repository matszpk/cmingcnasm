#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/utils/mem.h>
#include <ulinux/epoll.h>
#include <ulinux/file.h>
#include <ulinux/error.h>
#include <ulinux/signal/signal.h>

#define OUTC(s) sysc(write,3,0,s,sizeof(s))
#define SIGBIT(sig) (1<<(sig-1))
#define EPOLL_EVENTS_N 10

//don't let gcc optimize the thread entry function
static void __attribute__((noreturn,noinline,noclone)) thread_entry(void)
{
	while(1);
	sysc(exit,1,0);
}

void _start(void)
{
  //synchronous treatement of signals with signalfd
  k_u64 mask=(~0);//don't forget, you cannot even touch SIGKILL, neither SIGSTOP
  k_l r=sysc(rt_sigprocmask,4,K_SIG_BLOCK,&mask,0,sizeof(mask));
  if(K_ISERR(r)) sysc(exit_group,1,-1);

  mask=SIGBIT(K_SIGUSR1)|SIGBIT(K_SIGUSR2);
  k_i sigs_fd=(k_i)sysc(signalfd4,4,-1,&mask,sizeof(mask),K_SFD_NONBLOCK);
  if(K_ISERR(sigs_fd)) sysc(exit_group,1,-2);

  k_i epfd=(k_i)sysc(epoll_create1,1,0);
  if(K_ISERR(epfd)) sysc(exit_group,1,-3);

  struct k_epoll_event evts[EPOLL_EVENTS_N];
  u_memset(evts,0,sizeof(struct k_epoll_event));
  evts[0].events=K_EPOLLET|K_EPOLLIN;
  evts[0].data.fd=sigs_fd;
  r=sysc(epoll_ctl,4,epfd,K_EPOLL_CTL_ADD,sigs_fd,&evts[0]);
  if(K_ISERR(r)) sysc(exit_group,1,-4);

  while(1){
    do{
      u_memset(evts,0,sizeof(evts));
      r=sysc(epoll_wait,4,epfd,evts,EPOLL_EVENTS_N,-1);
    }while(r==-K_EINTR);
    if(K_ISERR(r)) sysc(exit_group,1,-6);

    for(k_l i=0;i<r;++i)
      if(evts[i].data.fd==sigs_fd){
        if(evts[i].events&K_EPOLLIN){
          struct k_signalfd_siginfo info;

          while(1){
            do{
              u_memset(&info,0,sizeof(info));
              r=sysc(read,3,sigs_fd,&info,sizeof(info));
            }while(r==-K_EINTR);
            if(r!=-K_EAGAIN&&((K_ISERR(r)||(r>0&&r!=sizeof(info)))))
              sysc(exit_group,1,-7);
            if(r==0||r==-K_EAGAIN) break;

            switch(info.ssi_signo){
            case K_SIGUSR1:
              OUTC("SIGUSR1\n");
              break;
            case K_SIGUSR2:
              OUTC("SIGUSR2\n");
              break;
            }
          }
        }else sysc(exit_group,1,-8);
      }
  }
}
