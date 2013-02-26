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

//XXX:what udev does in extra
// - it checks on socket credential
// - it checks the netlink sender is 0 (kernel)
// - it uses the socket filter

#define DPRINTF_BUF_SZ 1024
static k_u8 *g_dprintf_buf;
#define ERR(f,...) u_a_dprintf(2,g_dprintf_buf,DPRINTF_BUF_SZ,(k_u8*)f,\
                               __VA_ARGS__)
#define ERRC(s) sysc(write,3,2,s,sizeof(s))

static void uevent_rcv(k_i s)
{
  k_u8 buf[8192];
  u_memset(buf,0,sizeof(buf));

  struct k_io_vec io_vec;
  io_vec.base=buf;
  io_vec.len=sizeof(buf);

  struct k_msg_hdr msg;
  u_memset(&msg,0,sizeof(msg));
  msg.iov=&io_vec;
  msg.iov_len=1;

  k_l r;
  do{
    r=sysc(recvmsg,3,s,&msg,0);
  }while(r==-K_EINTR);
  if(K_ISERR(r)){
    ERR("ERROR:unable to receive the uevent(%ld)\n",r);
    sysc(exit_group,1,-1);
  }
  if(msg.flgs&K_MSG_TRUNC){
    ERR("ERROR:the uevent was truncated(flags=0x%x)\n",msg.flgs);
    sysc(exit_group,1,-1);
  }
  ERRC("uevent received:\n");
  k_i len=r;
  k_u8 *p=&buf[0];
  while(len){
    if(*p) ERR("%c",*p); else ERRC("\n");
    ++p;
	len--;
  }
  ERRC("\n");
}

void _start(void)
{
  static k_u8 dprintf_buf[DPRINTF_BUF_SZ];
  g_dprintf_buf=&dprintf_buf[0];

  //----------------------------------------------------------------------------

  k_i ep_fd=(k_i)sysc(epoll_create1,1,0);
  if(K_ISERR(ep_fd)){
    ERR("ERROR:unable to create epoll fd (%d)\n",ep_fd);
    sysc(exit_group,1,-1);
  }

  //----------------------------------------------------------------------------

  k_i s=(k_i)sysc(socket,3,K_PF_NETLINK,K_SOCK_RAW|K_SOCK_NONBLOCK,
                           K_NETLINK_KOBJECT_UEVENT);
  if(K_ISERR(s)){
    ERR("ERROR:unable to uevent netlink socket:%d\n",s);
    sysc(exit_group,1,-1);
  }

  //----------------------------------------------------------------------------

  //why that big (stolen from udev)?
  //moreover it can be skipped most of the time
  //must be priviledged
  k_i recv_buf_sz=128*1024*1024;
  k_l r=sysc(setsockopt,5,s,K_SOL_SOCKET,K_SO_RCVBUFFORCE,&recv_buf_sz,
                          sizeof(recv_buf_sz));
  if(K_ISERR(r)){
    ERR("ERROR:unable to force the size of the socket buffer (%ld)\n",r);
    sysc(exit_group,1,-1);
  }

  //----------------------------------------------------------------------------

  //uevent groups? only one: 1
  struct k_sockaddr_nl addr={K_AF_NETLINK,0,0,1};
  r=sysc(bind,3,s,&addr,sizeof(addr));
  if(K_ISERR(r)){
    ERR("ERROR:unable to bind address to uevent netlink socket:%ld\n",r);
    sysc(exit_group,1,-1);
  }
  
  //----------------------------------------------------------------------------
 
  struct timespec wanted={20,0};
  struct timespec rem={0,0};
  ERRC("Sleeping 20 sec (buffering)...\n");
  r=sysc(nanosleep,2,&wanted,&rem);
  if(r==-K_EINTR){
    ERR("WARNING:sleep was interruped. Remainder %ld sec/%ld nsec\n",rem.sec,
        rem.nsec);
  }else if(K_ISERR(r)){
    ERR("ERROR:sleeping:%ld\n",r);
    sysc(exit_group,1,-1);
  }
  ERRC("done\n");

  //----------------------------------------------------------------------------

  struct k_epoll_event ep_evt;
  u_memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=K_EPOLLIN;
  ep_evt.data.fd=s;
  r=sysc(epoll_ctl,4,ep_fd,K_EPOLL_CTL_ADD,s,&ep_evt);
  if(K_ISERR(r)){
    ERR("ERROR:unable to register uevent netlink socket to epoll (%ld)\n",r);
    sysc(exit_group,1,-1);
  }

  //----------------------------------------------------------------------------

  while(1){
    static struct k_epoll_event evts[1];//uevent netlink event
    do{
      u_memset(evts,0,sizeof(evts));
      r=sysc(epoll_wait,4,ep_fd,evts,1,-1);
    }while(r==-K_EINTR);
    if(K_ISERR(r)){
      ERR("ERROR:error epolling fds (%ld)\n",r);
      sysc(exit_group,1,-1);
    }
	k_l i=0;
	while(i<r){
      if(evts[i].data.fd==s){
        if(evts[i].events&K_EPOLLIN){
          uevent_rcv(s);
        }else{
          ERR("ERROR:unmanaged epolling event on uevent netlink socket"
              " n=%ld events=%u\n", i,evts[i].events);
          sysc(exit_group,1,-1);
        }
      }
      ++i;
    }
  }
  sysc(exit_group,1,0);
}
