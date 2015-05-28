//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#include <stdarg.h>
#include <ulinux/compiler_types.h>
#include <ulinux/sysc.h>
#include <ulinux/types.h>
#include <ulinux/error.h>
#include <ulinux/file.h>
#include <ulinux/stat.h>
#include <ulinux/mmap.h>
#include <ulinux/time.h>
#include <ulinux/signal/signal.h>
#include <ulinux/socket/socket.h>
#include <ulinux/socket/msg.h>
#include <ulinux/socket/netlink.h>
#include <ulinux/epoll.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/mem.h>

//------------------------------------------------------------------------------
//ulinux namespace
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define si ulinux_si
#define sl ulinux_sl
#define u8 ulinux_u8
#define memset ulinux_memset
#define io_vec ulinux_io_vec
#define msg_hdr ulinux_msg_hdr
#define recvmsg(a,b,c) ulinux_sysc(recvmsg,3,a,b,c)
#define ISERR ULINUX_ISERR
#define MSG_TRUNC ULINUX_MSG_TRUNC
#define exit(code) ulinux_sysc(exit_group,1,code)
#define epoll_create1(a) ulinux_sysc(epoll_create1,1,a)
#define socket(a,b,c) ulinux_sysc(socket,3,a,b,c)
#define NETLINK ULINUX_PF_NETLINK
#define RAW ULINUX_SOCK_RAW
#define NONBLOCK ULINUX_SOCK_NONBLOCK
#define NETLINK_KOBJECT_UEVENT ULINUX_NETLINK_KOBJECT_UEVENT
#define setsockopt(a,b,c,d,e) ulinux_sysc(setsockopt,5,a,b,c,d,e)
#define SOL_SOCKET ULINUX_SOL_SOCKET
#define SO_RCVBUFFORCE ULINUX_SO_RCVBUFFORCE
#define sockaddr_nl ulinux_sockaddr_nl
#define AF_NETLINK ULINUX_AF_NETLINK
#define bind(a,b,c) ulinux_sysc(bind,3,a,b,c)
#define nanosleep(a,b) ulinux_sysc(nanosleep,2,a,b)
#define epoll_event ulinux_epoll_event
#define epoll_ctl(a,b,c,d) ulinux_sysc(epoll_ctl,4,a,b,c,d)
#define ADD ULINUX_EPOLL_CTL_ADD
#define epoll_wait(a,b,c,d) ulinux_sysc(epoll_wait,4,a,b,c,d)
#define IN ULINUX_EPOLLIN
#define timespec ulinux_timespec
//------------------------------------------------------------------------------

//XXX:what udev does in extra
// - it checks on socket credential
// - it checks the netlink sender is 0 (kernel)
// - it uses the socket filter

#define DPRINTF_BUF_SZ 1024
static u8 *g_dprintf_buf;
#define ERR(fmt,...) ulinux_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,\
fmt,##__VA_ARGS__)

#define ERRC(str) {l rl;do rl=ulinux_sysc(write,3,2,str,sizeof(str));\
while(rl==-EINTR||rl==-EAGAIN);}

static void uevent_rcv(i sock)
{
  u8 buf[8192];
  memset(buf,0,sizeof(buf));

  struct io_vec io_vec;
  io_vec.base=buf;
  io_vec.len=sizeof(buf);

  struct msg_hdr msg;
  memset(&msg,0,sizeof(msg));
  msg.iov=&io_vec;
  msg.iov_len=1;

  l r;
  do r=recvmsg(sock,&msg,0); while(r==-EINTR);
  if(ISERR(r)){
    ERR("ERROR:unable to receive the uevent(%ld)\n",r);
    exit(-1);
  }
  if(msg.flgs&MSG_TRUNC){
    ERR("ERROR:the uevent was truncated(flags=0x%x)\n",msg.flgs);
    exit(-1);
  }
  ERRC("uevent received:\n");
  i len=r;
  u8 *p=&buf[0];
  while(len){
    if(*p){ERR("%c",*p);}else{ERRC("\n")};
    ++p;
	len--;
  }
  ERRC("\n");
}

void _start(void)
{
  static u8 dprintf_buf[DPRINTF_BUF_SZ];
  g_dprintf_buf=&dprintf_buf[0];

  //----------------------------------------------------------------------------

  i ep_fd=(i)epoll_create1(0);
  if(ISERR(ep_fd)){
    ERR("ERROR:unable to create epoll fd (%d)\n",ep_fd);
    exit(-1);
  }

  //----------------------------------------------------------------------------

  i so=(i)socket(NETLINK,RAW|NONBLOCK,NETLINK_KOBJECT_UEVENT);
  if(ISERR(so)){
    ERR("ERROR:unable to uevent netlink socket:%d\n",so);
    exit(-1);
  }

  //----------------------------------------------------------------------------

  //why that big (stolen from udev)?
  //moreover it can be skipped most of the time
  //must be priviledged
  i recv_buf_sz=128*1024*1024;
  l r=setsockopt(so,SOL_SOCKET,SO_RCVBUFFORCE,&recv_buf_sz,sizeof(recv_buf_sz));
  if(ISERR(r)){
    ERR("ERROR:unable to force the size of the socket buffer (%ld)\n",r);
    exit(-1);
  }

  //----------------------------------------------------------------------------

  //uevent groups? only one: 1
  struct sockaddr_nl addr={AF_NETLINK,0,0,1};
  r=bind(so,&addr,sizeof(addr));
  if(ISERR(r)){
    ERR("ERROR:unable to bind address to uevent netlink socket:%ld\n",r);
    exit(-1);
  }
  
  //----------------------------------------------------------------------------
 
  struct timespec wanted={20,0};
  struct timespec rem={0,0};
  ERRC("sleeping 20 sec (buffering)...\n");
  r=nanosleep(&wanted,&rem);
  if(r==-EINTR){
    ERR("WARNING:sleep was interruped. Remainder %ld sec/%ld nsec\n",rem.sec,
        rem.nsec);
  }else if(ISERR(r)){
    ERR("ERROR:sleeping:%ld\n",r);
    exit(-1);
  }
  ERRC("done\n");

  //----------------------------------------------------------------------------

  struct epoll_event ep_evt;
  memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=IN;
  ep_evt.data.fd=so;
  r=epoll_ctl(ep_fd,ADD,so,&ep_evt);
  if(ISERR(r)){
    ERR("ERROR:unable to register uevent netlink socket to epoll (%ld)\n",r);
    exit(-1);
  }

  //----------------------------------------------------------------------------

  while(1){
    static struct epoll_event evts[1];//uevent netlink event
    do{
      memset(evts,0,sizeof(evts));
      r=epoll_wait(ep_fd,evts,1,-1);
    }while(r==-EINTR);
    if(ISERR(r)){
      ERR("ERROR:error epolling fds (%ld)\n",r);
      exit(-1);
    }

    l j=0;
    while(1){
      if(j>=r) break;
      if(evts[j].data.fd==so){
         if(evts[j].events&IN){
           uevent_rcv(so);
         }else{
           ERR("ERROR:unmanaged epolling event on uevent netlink socket"
                                         " n=%ld events=%u\n",j,evts[j].events);
           exit(-1);
         }
      }
      ++j;
    }
  }
  exit(0);
}
