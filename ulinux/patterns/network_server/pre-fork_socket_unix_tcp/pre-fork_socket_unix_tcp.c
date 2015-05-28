//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//------------------------------------------------------------------------------
//|compiler stuff
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//|ulinux stuff
//------------------------------------------------------------------------------
#include <ulinux/compiler_types.h>
#include <ulinux/types.h>
#include <ulinux/sysc.h>

#include <ulinux/args_env.h>
#include <ulinux/file.h>
#include <ulinux/socket/socket.h>
#include <ulinux/socket/in.h>
#include <ulinux/socket/msg.h>
#include <ulinux/signal/signal.h>
#include <ulinux/error.h>
#include <ulinux/epoll.h>
#include <ulinux/mmap.h>
#include <ulinux/wait.h>

#include <ulinux/utils/atomic.h>
#include <ulinux/utils/ascii/ascii.h>
#include <ulinux/utils/ascii/string/string.h>
#include <ulinux/utils/ascii/string/vsprintf.h>
#include <ulinux/utils/ascii/string/conv/decimal/decimal.h>
#include <ulinux/utils/endian.h>
#include <ulinux/utils/ascii/block/conv/net/net.h>
#include <ulinux/utils/mem.h>

#include "ulinux_namespace.h"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|sockets stuff
//------------------------------------------------------------------------------
static struct sockaddr_in srv_addr={
  .sin_family=AF_INET,
  .sin_port=0,
  .sin_addr={INADDR_ANY}};
static i srv_sock=-1;//the main listening socket
static i cnx_sock=-1;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|non preclones stuff
//------------------------------------------------------------------------------
static u16 ps_max=0;//max of non preclone processes
static u16 ps_n=0;//current number of non preclone processes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|precloneS stuff
//------------------------------------------------------------------------------
struct preclone{
  i  sock;//unix socket to pass TCP sockets
  i  pid;
};
static struct preclone *preclones=0;//preclones management
static s16 preclones_n=0;//can be 0

#define PRECLONE_AVAILABLE 0
#define PRECLONE_BUSY      1
static atomic_u8_t *preclones_states=0;//shared array of atomic states

static struct msg_hdr msg;//msg to pass the connexion socket
//the control message buffer: control hdr long padded block, plus data long
//padded block
static u8 cmsg_buf[CMSG_SPACE(sizeof(cnx_sock))];
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|preclone stuff
//------------------------------------------------------------------------------
static u16 preclone_runs=0;//number of runs a preclone does before dying
static s16 preclone_slot=-1;
static i preclone_parent_sock=-1;//where to receive the fd of the connexion socket
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|fd facilities
//------------------------------------------------------------------------------
static i sigs_fd=-1;//fd for signals
static i epfd=-1;//epoll fd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|misc
//------------------------------------------------------------------------------
static u32* allows;
static u32 allow_n=0;//ipv4 address to allow traffic from
#define DPRINTF_BUF_SZ 1024
static u8 *dprintf_buf;
#define POUTC(str) {l rl;do{rl=ulinux_sysc(write,3,0,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

#define PERRC(str) {l rl;do{rl=ulinux_sysc(write,3,2,str,sizeof(str));}\
while(rl==-EINTR||rl==-EAGAIN);}

#define PERR(fmt,...) ulinux_dprintf(2,dprintf_buf,DPRINTF_BUF_SZ,fmt,\
##__VA_ARGS__)
#define SIGBIT(sig) (1<<(sig-1))
//------------------------------------------------------------------------------

//******************************************************************************
//******************************************************************************
//******************************************************************************

static void usage(void)
{
  POUTC("-inet <x.x.x.x IPv4 address to listen to>\n"
        "-port <TCP port to listen to>\n"
        "-psmax <max number of non precloned processes\n"
        "        default in 0, max is 65536>\n"
        "-preclones <number of precloned processes\n"
        "            default is 0, max is 65535>\n"
        "    -runs <number of runs a preclones does before dying\n"
        "           default is infinite,max is 65535>\n"
        "-allows <coma separated list of IPv4 adresses, x0.x0.x0.x0,x1.x1.x1.x1...\n"
        "         default is all ipv4 addresses accepted>\n"
        "-help display this text\n");
}

//******************************************************************************

static void cnx_sendrecv_setup(void)
{
  memset(&msg,0,sizeof(msg));
  memset(cmsg_buf,0,sizeof(cmsg_buf));
  
  msg.ctl=cmsg_buf;
  msg.ctl_len=sizeof(cmsg_buf);//long padded size for hdr
                               //plus long padded size for data
  struct cmsg_hdr *cmsg=CMSG_FIRSTHDR(&msg);
  cmsg->lvl=SOL_SOCKET;
  cmsg->type=SCM_RIGHTS;
  cmsg->len=CMSG_LEN(sizeof(cnx_sock));//long padded size for hdr plus
                                       //*not* long padded size for data
  //XXX:was wrong but had worked
  //msg.ctl_len=cmsg->len;//long padded for hdr plus
                          //*not* long padded size for data
                          //that means to ignore the padding bytes at the end
                          //of the data block
}

//******************************************************************************
//*clone stuff
//******************************************************************************
static void clone_sigs_setup(void)
{
  //cannot change SIGKILL, neither SIGSTOP
  u64 mask=(~0);
  l r=rt_sigprocmask(SIG_BLOCK,&mask,0,sizeof(mask));
  if(ISERR(r)){
    PERR("FATAL:child:clone:error blocking mostly all signals (%ld)\n",r);
    exit(-1);
  }
}

static void clone_cnx_do(void)
{
  POUTC("CLONE:DOING SOME WORK!\n");
  l r;
  do r=close(cnx_sock); while(r==-EINTR);
}

static void clone_setup(void)
{
  clone_sigs_setup();

  l r=dup2(cnx_sock,0);
  if(ISERR(r)){
    PERR("FATAL:child:clone:unable to make stdin the connexion socket (%ld)\n",
                                                                             r);
    exit(-1);
  }

  r=dup2(cnx_sock,1);
  if(ISERR(r)){
    PERR("FATAL:child:clone:unable to make stdout the connexion socket (%ld)\n",
                                                                             r);
    exit(-1);
  }
}

static void clone_entry(void)
{
  clone_setup();
  clone_cnx_do();
  exit(0);
}
//******************************************************************************


//******************************************************************************
//*preclone stuff (if too much, will go away in another source file)
//*fds mapping:
//* - 0 connexion socket
//* - 1 connexion socket
//* - 2 parent stderr
//* - 3 epoll
//* - 4 signalfd fd
//* - 5 parent unix socket for receiving connexion socket
//******************************************************************************
static void preclone_sigs_setup(void)
{
  //cannot change SIGKILL, neither SIGSTOP
  ul mask=(~0);
  l r=rt_sigprocmask(SIG_BLOCK,&mask,0,sizeof(mask));
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:error blocking mostly all signals (%ld)\n",
                                                               preclone_slot,r);
    exit(-1);
  }

  mask=SIGBIT(SIGTERM);
  i sigs_fd=(i)signalfd4(-1,&mask,sizeof(mask),SFD_NONBLOCK);
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:error setting up fd for signals (%d)\n",
                                                         preclone_slot,sigs_fd);
    exit(-1);
  }

  if(sigs_fd!=4){//moving signalfd fd to 4, if required
    r=dup2(sigs_fd,4);
    if(ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move signalfd fd (%ld)\n",
                                                               preclone_slot,r);
      exit(-1);
    }
    do r=close(sigs_fd); while(r==-EINTR);
  }
}

static void preclone_epoll_setup(void)
{
  i epfd=(i)epoll_create1(0);
  if(ISERR(epfd)){
    PERR("FATAL:child:preclone:%d:unable to create epoll fd (%d)\n",
                                                            preclone_slot,epfd);
    exit(-1);
  }

  if(epfd!=3){//moving epoll fd to 3, if required
    l r=dup2(epfd,3);
    if(ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move epoll fd (%ld)\n",
                                                               preclone_slot,r);
      exit(-1);
    }
    do r=close(epfd); while(r==-EINTR);
  }

  struct epoll_event ep_evt;
  memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=EPOLLET|EPOLLIN;

  ep_evt.data.fd=4;
  l r=epoll_ctl(3,EPOLL_CTL_ADD,4,&ep_evt);
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to register signal fd to epoll (%ld)\n",
                                                               preclone_slot,r);
    exit(-1);
  }

  ep_evt.data.fd=5;
  ep_evt.events=EPOLLIN|EPOLLPRI;
  r=epoll_ctl(3,EPOLL_CTL_ADD,5,&ep_evt);
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to register parent socket to epoll"
                                                    " (%ld)\n",preclone_slot,r);
    exit(-1);
  }
}

static void preclone_cnx_get(void)
{
  cnx_sendrecv_setup();

  l r;
  do r=recvmsg(5,&msg,0); while(r==-EINTR);
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to get the connexion from parent (%ld)"
                                                          "\n",preclone_slot,r);
    exit(-1);
  }
  if(msg.flgs&MSG_CTRUNC){
    PERR("FATAL:child:preclone:%d:buffer error getting the connexion from"
                                     " parent (0x%x)\n",preclone_slot,msg.flgs);
    exit(-1);
  }

  i cnx_sock=*((i*)(CMSG_DATA(CMSG_FIRSTHDR(&msg))));

  if(cnx_sock!=0){//moving connexion socket fd to 0 if required
    r=dup2(cnx_sock,0);
    if(ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move socket connexion fd (%ld)\n",
                                                               preclone_slot,r);
      exit(-1);
    }
    do r=close(cnx_sock); while(r==-EINTR);
  }

  r=dup2(0,1);//stdout is connexion socket, 1 was already closed
  if(ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to duplicate connexion socket on"
                                             " stdout (%ld)\n",preclone_slot,r);
    exit(-1);
  }
}

static void preclone_cnx_do(void)
{
  preclone_cnx_get();
  POUTC("PRE-CLONE:DOING SOME WORK!\n");
  l r;
  do r=close(0); while(r==-EINTR);
  do r=close(1); while(r==-EINTR);
  if(preclone_runs) if(!--preclone_runs) exit(0);//time to die
  atomic_u8_set(preclones_states+preclone_slot,PRECLONE_AVAILABLE);
}

static void preclone_sigs_consume(void)
{
  static struct signalfd_siginfo info;

  while(1){
    l r;
    do r=read(4,&info,sizeof(info)); while(r==-EINTR);
    if(r!=-EAGAIN&&(ISERR(r)||(r>0&&r!=sizeof(info)))){
      PERR("FATAL:child:preclone:%d:error consuming signals (%ld)\n",
                                                               preclone_slot,r);
      exit(-1);
    }
    if(r==0||r==-EAGAIN) break;

    switch(info.ssi_signo){
    case SIGTERM:
      exit(0);
    }
  }
}

static void preclone_loop(void)
{
  while(1){
    static struct epoll_event evts[3];
    l r;
    do{
      memset(evts,0,sizeof(evts));
      r=epoll_wait(3,evts,3,-1);
    }while(r==-EINTR);
    if(ISERR(r)){
      PERR("FATAL:child:preclone:%d:error epolling fds (%ld)\n",preclone_slot,
                                                                             r);
      exit(-1);
    }
    for(l j=0;j<r;++j)
      if(evts[j].data.fd==4){
        if(evts[j].events&EPOLLIN) preclone_sigs_consume();
        else{
          PERR("FATAL:child:preclone:%d:unknown epolling event on signal fd"
                           " n=%ld events=%u\n",preclone_slot,j,evts[j].events);
          exit(-1);
        }
      }else if(evts[j].data.fd==5){
        if(evts[j].events&(EPOLLERR|EPOLLHUP|EPOLLPRI)){
          PERR("FATAL:child:preclone:%d:something wrong with parent socket"
                  " epolling n=%ld events=%u\n",preclone_slot,j,evts[j].events);
          exit(-1);
        }else if(evts[j].events&EPOLLIN){
          preclone_cnx_do();
        }else{
          PERR("FATAL:child:preclone:%d:unknown epolling event on parent socket"
                           " n=%ld events=%u\n",preclone_slot,j,evts[j].events);
          exit(-1);
        }
      }
  }
}

static void preclone_setup(void)
{
  if(preclone_parent_sock!=5){//moving parent socket fd to 5, if requiered
    l r=dup2(preclone_parent_sock,5);
    if(ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move parent socket fd (%ld)\n",
                                                               preclone_slot,r);
      exit(-1);
    }
    do r=close(preclone_parent_sock); while(r==-EINTR);
  }

  preclone_sigs_setup();
  preclone_epoll_setup();
  memset(&msg,0,sizeof(msg));
}

static void preclone_entry(void)
{
  preclone_setup();
  atomic_u8_set(preclones_states+preclone_slot,PRECLONE_AVAILABLE);
  preclone_loop();
}
//******************************************************************************

static void clone_spawn(void)
{
  l r;
  if(ps_n<ps_max){
    r=clone(SIGCHLD,0,0,0,0);
    if(ISERR(r)){
      PERR("FATAL:error cloning %ld\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }
    if(r==0) goto clone_entry_setup;
 
    ++ps_n;
  }
  return;

clone_entry_setup:
  do r=close(0); while(r==-EINTR);//for the connexion socket
  do r=close(1); while(r==-EINTR);//for the connexion socket
  //keep stderr

  do r=close(sigs_fd); while(r==-EINTR);//don't care of parent signal
  do r=close(epfd); while(r==-EINTR);//don't use parent epoll

  clone_entry();
  return;
}

static void preclone_ctl_socket(i sockets[2])
{
  l r=socketpair(AF_UNIX,SOCK_DGRAM,0,sockets);
  if(ISERR(r)){
    PERR("FATAL:error creating preclone socket pair (%ld)\n",r);
    kill(0,SIGTERM);
    exit(-1);
  }
}

static i preclone_spawn(s16 slot)
{
  atomic_u8_set(preclones_states+slot,PRECLONE_BUSY);

  i sockets[2];
  preclone_ctl_socket(sockets);

  l r=clone(SIGCHLD,0,0,0,0);
  if(ISERR(r)){
    PERR("FATAL:error precloning process slot %d with error %ld\n",slot,r);
    return -1;
  }
  if(r==0) goto preclone_entry_setup;
 
  preclones[slot].sock=sockets[1];
  do r=close(sockets[0]); while(r==-EINTR);
  preclones[slot].pid=(i)r; 
  return 0;

preclone_entry_setup:
  preclone_slot=slot;
  do r=close(sockets[1]); while(r==-EINTR);//close parent unix socket
  preclone_parent_sock=sockets[0];

  do r=close(0); while(r==-EINTR);//for the connexion socket
  do r=close(1); while(r==-EINTR);//for the connexion socket
  //keep stderr

  do r=close(sigs_fd); while(r==-EINTR);//don't care of parent signal
  do r=close(epfd); while(r==-EINTR);//don't use parent epoll

  preclone_entry();
  return -1;
}

static void sigs_setup(void)
{//synchronous treatement of signals with signalfd
  //cannot change SIGKILL, neither SIGSTOP
  ul mask=(~0);
  l r=rt_sigprocmask(SIG_BLOCK,&mask,0,sizeof(mask));
  if(ISERR(r)){
    PERR("FATAL:error blocking mostly all signals (%ld)\n",r);
    exit(-1);
  }

  mask=SIGBIT(SIGTERM)|SIGBIT(SIGCHLD);
  sigs_fd=(i)signalfd4(-1,&mask,sizeof(mask),SFD_NONBLOCK);
  if(ISERR(sigs_fd)){
    PERR("FATAL:error setting up fd for signals (%d)\n",sigs_fd);
    exit(-1);
  }
}

static void mem_allocate(void)
{
  if(!preclones_n) return;//nothing to allocate

  l r=mmap(0,(preclones_n)*sizeof(&preclones),PROT_READ|PROT_WRITE,
                                    MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE,0,0);
  if(!r||ISERR(r)){
    PERR("FATAL:unable to allocate children infos memory:%ld\n",r);
    exit(-1);
  }
  preclones=(struct preclone *)r;

  r=mmap(0,(preclones_n)*sizeof(&preclones_states),PROT_READ|PROT_WRITE,
                                     MAP_SHARED|MAP_ANONYMOUS|MAP_POPULATE,0,0);
  if(!r||ISERR(r)){
    PERR("FATAL:unable to allocate children shared memory:%ld\n",r);
    exit(-1);
  }
  preclones_states=(atomic_u8_t *)r;
  for(s16 slot=0;slot<preclones_n;++slot)
    atomic_u8_set(preclones_states+slot,PRECLONE_AVAILABLE);
}

static void srv_sock_create(void)
{
  //TCP on IPv4... erk!
  srv_sock=(i)socket(PF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
  if(ISERR(srv_sock)){
    PERR("FATAL:unable to create the server listening socket:%d\n",srv_sock);
    kill(0,SIGTERM);
    exit(-1);
  }

  l bool_true=1;
  l r=setsockopt(srv_sock,SOL_SOCKET,SO_REUSEADDR,&bool_true,sizeof(bool_true));
  if(ISERR(r)){
    PERR("FATAL:unable to turn on socket rebinding option:%ld\n",r);
    kill(0,SIGTERM);
    exit(-1);
  }

  r=bind(srv_sock,&srv_addr,sizeof(srv_addr));
  if(ISERR(r)){
    PERR("FATAL:unable to bind address/port on server listening socket:%ld\n",r);
    kill(0,SIGTERM);
    exit(-1);
  }

  r=listen(srv_sock,0);
  if(ISERR(r)){
    PERR("FATAL:unable to flag server listening socket:%ld\n",r);
    kill(0,SIGTERM);
    exit(-1);
  }
}

static void epoll_sigs_setup(void)
{
  epfd=(i)epoll_create1(0);
  if(ISERR(epfd)){
    PERR("FATAL:unable to create epoll fd (%d)\n",epfd);
    exit(-1);
  }
  struct epoll_event ep_evt;
  memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=EPOLLET|EPOLLIN;
  ep_evt.data.fd=sigs_fd;
  l r=epoll_ctl(epfd,EPOLL_CTL_ADD,sigs_fd,&ep_evt);
  if(ISERR(r)){
    PERR("FATAL:unable to register signal fd to epoll (%ld)\n",r);
    exit(-1);
  }
}

static void epoll_srv_sock_setup(void)
{
  struct epoll_event ep_evt;
  memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=EPOLLIN|EPOLLPRI;
  ep_evt.data.fd=srv_sock;
  l r=epoll_ctl(epfd,EPOLL_CTL_ADD,srv_sock,&ep_evt);
  if(ISERR(r)){
    PERR("FATAL:unable to register server socket to epoll (%ld)\n",r);
    kill(0,SIGTERM);
    exit(-1);
  }
}

static void preclones_spawn(void)
{
  for(s16 slot=0;slot<preclones_n;++slot)
    if(preclone_spawn(slot)<0){
      kill(0,SIGTERM);
      exit(-1);
    }
}

static void setup(void)
{
  mem_allocate();
  sigs_setup();
  epoll_sigs_setup();
  preclones_spawn();
  //from there, do not forget to kill the children (configured as no zombie)
  srv_sock_create();
  epoll_srv_sock_setup();
}

static void cmdline_options_allow_parse(void *list)
{
  u8 *start=(u8*)list;
  while(*start){
    u8 *end=start;
    while(*end&&*end!=',') ++end;
    if(!str_2ipv4_blk((u32*)list+allow_n,start,end-1)){
      PERR("FATAL:unable to convert IPv4 address from ascii to kernel format"
                                                 " (address n:%d)\n",allow_n+1);
      exit(-1);
    }
    ++allow_n;
    if(!*end) break;
    start=end+1;
  }
}

static void cmdline_options(l argc,void *argv[])
{
  #define OPTION(string) strncmp(string,argv[j],sizeof(string)-1)==0
  #define CHECK_ARG if(argc<=j+1){usage();exit(0);}

  for(i j=1;j<argc;j+=2){
    if(OPTION("-inet")){
      CHECK_ARG

      u64 len=0;
      if(!strnlen(&len,argv[j+1],sizeof("xxx.xxx.xxx.xxx")-1)){
        PERR("FATAL:invalid listening IPv4 address(size):%s\n",argv[j+1]);
        exit(-1);
      }
      if(!str_2ipv4_blk((u32*)&srv_addr.sin_addr,argv[j+1],argv[j+1]+len-1)){
        PERR("FATAL:invalid listening IPv4 address(format):%s\n",argv[j+1]);
        exit(-1);
      }
    }else if(OPTION("-port")){
      CHECK_ARG

      if(!dec2u16(&srv_addr.sin_port,argv[j+1])){
        PERR("FATAL:failed network port conversion:%s\n",argv[j+1]);
        exit(-1);
      }
      srv_addr.sin_port=cpu2be16(srv_addr.sin_port);
    }else if(OPTION("-psmax")){
      CHECK_ARG

      if(!dec2u16(&ps_max,argv[j+1])){
        PERR("FATAL:failed maximum number of non precloned processes"
                                                  " conversion:%s\n",argv[j+1]);
        exit(-1);
      }
    }else if(OPTION("-preclones")){
      CHECK_ARG

      u16 arg;
      if(!dec2u16(&arg,argv[j+1])){
        PERR("FATAL:failed number of precloned processes conversion:%s\n",
                                                                     argv[j+1]);
        exit(-1);
      }
      preclones_n=(s16)arg;
    }else if(OPTION("-allows")){
      CHECK_ARG

      cmdline_options_allow_parse(argv[j+1]);
      allows=(u32*)argv[j+1];
    }else if(OPTION("-runs")){
      CHECK_ARG

      if(!dec2u16(&preclone_runs,argv[j+1])){
        PERR("FATAL:failed number of runs conversion:%s\n",argv[j+1]);
        exit(-1);
      }
    }else{//"-help"
      usage();
      exit(0);
    }
  }
}

static void sigchld(void)
{
  while(1){
    l r;
    do r=wait4(-1,0,WNOHANG|WALL,0); while(r==-EINTR);
    //ECHILD seems to means not even 1 child to wait for
    if(r!=-ECHILD&&ISERR(r)){
      PERR("FATAL:error waiting on dead children (%ld)\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }
    if(r==0||r==-ECHILD) break;

    s16 slot;
    for(slot=0;slot<preclones_n;++slot) if(preclones[slot].pid==(i)r) break;

    if(slot<preclones_n){
      do r=close(preclones[slot].sock); while(r==-EINTR);
      if(preclone_spawn(slot)<0){
        kill(0,SIGTERM);
        exit(-1);
      }
    }else ps_n--;//a not precloned process died
  }
}

static void sigs_consume(void)
{
  static struct signalfd_siginfo info;

  while(1){
    l r;
    do{
      memset(&info,0,sizeof(info));
      r=read(sigs_fd,&info,sizeof(info));
    }while(r==-EINTR);
    if(r!=-EAGAIN&&((ISERR(r)||(r>0&&r!=sizeof(info))))){
      PERR("FATAL:error consuming signals (%ld)\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }
    if(r==0||r==-EAGAIN) break;

    switch(info.ssi_signo){
    case SIGCHLD:
      sigchld();
      break;
    case SIGTERM:
      kill(0,SIGTERM);
      exit(0);
    }
  }
}

static void cnx_send(s16 slot)
{
  cnx_sendrecv_setup();
  *((i*)(CMSG_DATA(CMSG_FIRSTHDR(&msg))))=cnx_sock;

  l r=sendmsg(preclones[slot].sock,&msg,0);
  if(ISERR(r)){
    PERR("FATAL:error sending connexion socket to preclone %d (%ld)\n",slot,r);
    kill(0,SIGTERM);
    exit(0);
  }
}

static void cnx_dispatch(void)
{
  s16 slot;
  for(slot=0;slot<preclones_n;++slot)
    if(atomic_u8_read(preclones_states+slot)==PRECLONE_AVAILABLE) break;

  if(slot<preclones_n){
    atomic_u8_set(preclones_states+slot,PRECLONE_BUSY);
    cnx_send(slot);
  }else clone_spawn();
  l r;
  do r=close(cnx_sock); while(r==-EINTR);
}

static u8 peer_is_allowed(u32 ipv4)
{
  for(u32 j=0;j<allow_n;++j) if(allows[j]==ipv4) return 1;
  return 0;
}

static void cnxs_consume(void)
{
  while(1){
    l r;
    struct sockaddr_in peer;
    l peer_len=sizeof(peer);
    do r=accept(srv_sock,&peer,&peer_len); while(r==-EINTR||r==-ECONNABORTED);

    if(r!=-EAGAIN&&ISERR(r)){//error
      PERR("FATAL:error accepting connexion:%ld\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }
    if(peer_len!=sizeof(peer)){
      PERR("FATAL:error accepting connexion(socket address size):%ld\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }

    if(r==-EAGAIN) break;//no more connexion pending

    cnx_sock=(i)r;
    if(allow_n){//allow filter is unabled if at least one address was provided
      if(peer_is_allowed(peer.sin_addr.s_addr)) cnx_dispatch();
      else do r=close(cnx_sock); while(r==-EINTR);
    }else cnx_dispatch();
  }
}

static void loop(void)
{
  while(1){
    static struct epoll_event evts[2];//sigs_fd and srv_sock
    l r;
    do{
      memset(evts,0,sizeof(evts));
      r=epoll_wait(epfd,evts,2,-1);
    }while(r==-EINTR);
    if(ISERR(r)){
      PERR("FATAL:error epolling fds (%ld)\n",r);
      kill(0,SIGTERM);
      exit(-1);
    }
    for(l j=0;j<r;++j)
      if(evts[j].data.fd==sigs_fd){
        if(evts[j].events&EPOLLIN) sigs_consume();
        else{
          PERR("FATAL:something wrong with signal fd epolling n=%ld"
                                               " events=%u\n",j,evts[j].events);
          kill(0,SIGTERM);
          exit(-1);
        }
      }else if(evts[j].data.fd==srv_sock){
        if(evts[j].events&(EPOLLERR|EPOLLHUP|EPOLLPRI)){
          PERR("FATAL:something wrong with server socket epolling n=%ld"
                                               " events=%u\n",j,evts[j].events);
          kill(0,SIGTERM);
          exit(-1);
        }else if(evts[j].events&EPOLLIN){
          cnxs_consume();
        }else{
          PERR("FATAL:unknown epolling event on server socket n=%ld"
                                               " events=%u\n",j,evts[j].events);
          kill(0,SIGTERM);
          exit(-1);
        }
      }
  }
}

//*****************************************************************************
//XXX:may do the daemonic stuff if really we need it:logger instead of tty,
//    own session...
void ulinux_start(l argc,void **argv)
{
  u8 buf[DPRINTF_BUF_SZ];
  dprintf_buf=&buf[0];

  cmdline_options(argc,argv);
  setup();
  loop();
}
