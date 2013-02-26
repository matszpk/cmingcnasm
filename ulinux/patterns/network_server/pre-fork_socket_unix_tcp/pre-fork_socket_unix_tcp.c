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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|sockets stuff
//------------------------------------------------------------------------------
static struct k_sockaddr_in _serv_addr={
  .sin_family=K_AF_INET,
  .sin_port=0,
  .sin_addr={K_INADDR_ANY}};
static k_i _serv_sock=-1;//the main listening socket
static k_i _cnx_sock=-1;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|non preclones stuff
//------------------------------------------------------------------------------
static k_us _ps_max=0;//max of non preclone processes
static k_us _ps_n=0;//current number of non preclone processes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|precloneS stuff
//------------------------------------------------------------------------------
struct preclone{
  k_i  s;//unix socket to pass TCP sockets
  k_i  pid;
};
static struct preclone *_preclones=0;//preclones management
static k_us _preclones_n=0;//can be 0

#define PRECLONE_AVAILABLE 0
#define PRECLONE_BUSY      1
static k_atomic_u8_t *_preclones_states=0;//shared array of atomic states

static struct k_msg_hdr _msg;//msg to pass the connexion socket
//the control message buffer: control hdr long padded block, plus data long
//padded block
static k_u8 _cmsg_buf[K_CMSG_SPACE(sizeof(_cnx_sock))];
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|preclone stuff
//------------------------------------------------------------------------------
static k_us _runs=0;//number of runs a preclone does before dying
static k_i _slot=-1;
static k_i _parent_sock=-1;//where to receive the fd of the connexion socket
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|fd facilities
//------------------------------------------------------------------------------
static k_i _sigs_fd=-1;//fd for signals
static k_i _epfd=-1;//epoll fd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//|misc
//------------------------------------------------------------------------------
static k_u32* _allows;
static k_i _allow_n=0;//ipv4 address to allow traffic from
#define _DPRINTF_BUF_SZ 1024
static k_u8 *_dprintf_buf;
#define POUTC(s) {k_l rl;do{rl=sysc(write,3,0,s,sizeof(s));}\
                         while(rl==-K_EINTR||rl==-K_EAGAIN);}
#define PERRC(s) {k_l rl;do{rl=sysc(write,3,2,s,sizeof(s));}\
                         while(rl==-K_EINTR||rl==-K_EAGAIN);}
#define PERR(f,...) u_a_dprintf(2,_dprintf_buf,_DPRINTF_BUF_SZ,(k_u8*)f,\
                                __VA_ARGS__)
#define SIGBIT(sig) (1<<(sig-1))
//------------------------------------------------------------------------------

//******************************************************************************
//******************************************************************************
//******************************************************************************

static void _usage(void)
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

static void _cnx_sendrecv_setup(void)
{
  u_memset(&_msg,0,sizeof(_msg));
  u_memset(_cmsg_buf,0,sizeof(_cmsg_buf));
  
  _msg.ctl=_cmsg_buf;
  _msg.ctl_len=sizeof(_cmsg_buf);//long padded size for hdr
                                     //plus long padded size for data
  struct k_cmsg_hdr *cmsg=K_CMSG_FIRSTHDR(&_msg);
  cmsg->lvl=K_SOL_SOCKET;
  cmsg->type=K_SCM_RIGHTS;
  cmsg->len=K_CMSG_LEN(sizeof(_cnx_sock));//long padded size for hdr plus
                                          //*not* long padded size for data
  //XXX:was wrong but had worked
  //_msg.ctl_len=cmsg->len;//long padded for hdr plus
                           //*not* long padded size for data
                           //that means to ignore the padding bytes at the end
                           //of the data block
}

//******************************************************************************
//*clone stuff
//******************************************************************************
static void _clone_sigs_setup(void)
{
  //cannot change SIGKILL, neither SIGSTOP
  k_u64 mask=(~0);
  k_l r=sysc(rt_sigprocmask,4,K_SIG_BLOCK,&mask,0,sizeof(mask));
  if(K_ISERR(r)){
    PERR("FATAL:child:clone:error blocking mostly all signals (%ld)\n",r);
    sysc(exit_group,1,-1);
  }
}

static void _clone_cnx_do(void)
{
  POUTC("CLONE:DOING SOME WORK!\n");
  sysc(close,1,_cnx_sock);
}

static void _clone_setup(void)
{
  _clone_sigs_setup();

  k_l r=sysc(dup2,2,_cnx_sock,0);
  if(K_ISERR(r)){
    PERR("FATAL:child:clone:unable to make stdin the connexion socket (%ld)\n",
         r);
    sysc(exit_group,1,-1);
  }

  r=sysc(dup2,2,_cnx_sock,1);
  if(K_ISERR(r)){
    PERR("FATAL:child:clone:unable to make stdout the connexion socket (%ld)\n",
         r);
    sysc(exit_group,1,-1);
  }
}

static void _clone_entry(void)
{
  _clone_setup();
  _clone_cnx_do();
  sysc(exit_group,1,0);
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
static void _preclone_sigs_setup(void)
{
  //cannot change SIGKILL, neither SIGSTOP
  k_ul mask=(~0);
  k_l r=sysc(rt_sigprocmask,4,K_SIG_BLOCK,&mask,0,sizeof(mask));
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:error blocking mostly all signals (%ld)\n",
         _slot,r);
    sysc(exit_group,1,-1);
  }

  mask=SIGBIT(K_SIGTERM);
  k_i sigs_fd=(k_i)sysc(signalfd4,4,-1,&mask,sizeof(mask),K_SFD_NONBLOCK);
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:error setting up fd for signals (%d)\n",
         _slot,sigs_fd);
    sysc(exit_group,1,-1);
  }

  if(sigs_fd!=4){//moving signalfd fd to 4, if requiered
    r=sysc(dup2,2,sigs_fd,4);
    if(K_ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move signalfd fd (%ld)\n",
           _slot,r);
      sysc(exit_group,1,-1);
    }
    sysc(close,1,sigs_fd);
  }
}

static void _preclone_epoll_setup(void)
{
  k_i epfd=(int)sysc(epoll_create1,1,0);
  if(K_ISERR(_epfd)){
    PERR("FATAL:child:preclone:%d:unable to create epoll fd (%d)\n",_slot,epfd);
    sysc(exit_group,1,-1);
  }

  if(epfd!=3){//moving epoll fd to 3, if requiered
    k_l r=sysc(dup2,2,epfd,3);
    if(K_ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move epoll fd (%ld)\n",_slot,r);
      sysc(exit_group,1,-1);
    }
    sysc(close,1,epfd);
  }

  struct k_epoll_event ep_evt;
  u_memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=K_EPOLLET|K_EPOLLIN;

  ep_evt.data.fd=4;
  k_l r=sysc(epoll_ctl,4,3,K_EPOLL_CTL_ADD,4,&ep_evt);
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to register signal fd to epoll (%ld)\n",
         _slot,r);
    sysc(exit_group,1,-1);
  }

  ep_evt.data.fd=5;
  ep_evt.events=K_EPOLLIN|K_EPOLLPRI;
  r=sysc(epoll_ctl,4,3,K_EPOLL_CTL_ADD,5,&ep_evt);
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to register parent socket to epoll"
         " (%ld)\n",_slot,r);
    sysc(exit_group,1,-1);
  }
}

static void _preclone_cnx_get(void)
{
  _cnx_sendrecv_setup();

  k_l r;
  do{
    r=sysc(recvmsg,3,5,&_msg,0);
  }while(r==-K_EINTR);
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to get the connexion from parent (%ld)"
         "\n",_slot,r);
    sysc(exit_group,1,-1);
  }
  if(_msg.flgs&K_MSG_CTRUNC){
    PERR("FATAL:child:preclone:%d:buffer error getting the connexion from"
         " parent (0x%x)\n",_slot,_msg.flgs);
    sysc(exit_group,1,-1);
  }

  k_i cnx_sock=*((k_i*)(K_CMSG_DATA(K_CMSG_FIRSTHDR(&_msg))));

  if(cnx_sock!=0){//moving connexion socket fd to 0 if requiered
    r=sysc(dup2,2,cnx_sock,0);
    if(K_ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move socket connexion fd (%ld)\n",
           _slot,r);
      sysc(exit_group,1,-1);
    }
    sysc(close,1,cnx_sock);
  }

  r=sysc(dup2,2,0,1);//stdout is connexion socket, 1 was already closed
  if(K_ISERR(r)){
    PERR("FATAL:child:preclone:%d:unable to duplicate connexion socket on"
         " stdout (%ld)\n",_slot,r);
    sysc(exit_group,1,-1);
  }
}

static void _preclone_cnx_do(void)
{
  _preclone_cnx_get();
  POUTC("PRE-CLONE:DOING SOME WORK!\n");
  sysc(close,1,0);
  sysc(close,1,1);
  if(_runs) if(!--_runs) sysc(exit_group,1,0);//time to die
  k_atomic_u8_set(_preclones_states+_slot,PRECLONE_AVAILABLE);
}

static void _preclone_sigs_consume(void)
{
  static struct k_signalfd_siginfo info;

  while(1){
    k_l r;
    do{
      r=sysc(read,3,4,&info,sizeof(info));
    }while(r==-K_EINTR);
    if(r!=-K_EAGAIN&&(K_ISERR(r)||(r>0&&r!=sizeof(info)))){
      PERR("FATAL:child:preclone:%d:error consuming signals (%ld)\n",_slot,r);
      sysc(exit_group,1,-1);
    }
    if(r==0||r==-K_EAGAIN) break;

    switch(info.ssi_signo){
    case K_SIGTERM:
      sysc(exit_group,1,0);
    }
  }
}

static void _preclone_loop(void)
{
  while(1){
    static struct k_epoll_event evts[3];
    k_l r;
    do{
      u_memset(evts,0,sizeof(evts));
      r=sysc(epoll_wait,4,3,evts,3,-1);
    }while(r==-K_EINTR);
    if(K_ISERR(r)){
      PERR("FATAL:child:preclone:%d:error epolling fds (%ld)\n",_slot,r);
      sysc(exit_group,1,-1);
    }
    for(k_l i=0;i<r;++i)
      if(evts[i].data.fd==4){
        if(evts[i].events&K_EPOLLIN) _preclone_sigs_consume();
        else{
          PERR("FATAL:child:preclone:%d:"
               "unknown epolling event on signal fd n=%ld events=%u\n",
               _slot,i,evts[i].events);
          sysc(exit_group,1,-1);
        }
      }else if(evts[i].data.fd==5){
        if(evts[i].events&(K_EPOLLERR|K_EPOLLHUP|K_EPOLLPRI)){
          PERR("FATAL:child:preclone:%d:"
               "something wrong with parent socket epolling n=%ld events=%u\n",
               _slot,i,evts[i].events);
          sysc(exit_group,1,-1);
        }else if(evts[i].events&K_EPOLLIN){
          _preclone_cnx_do();
        }else{
          PERR("FATAL:child:preclone:%d:"
               "unknown epolling event on parent socket n=%ld events=%u\n",
               _slot,i,evts[i].events);
          sysc(exit_group,1,-1);
        }
      }
  }
}

static void _preclone_setup(void)
{
  if(_parent_sock!=5){//moving parent socket fd to 5, if requiered
    k_l r=sysc(dup2,2,_parent_sock,5);
    if(K_ISERR(r)){
      PERR("FATAL:child:preclone:%d:unable to move parent socket fd (%ld)\n",_slot,r);
      sysc(exit_group,1,-1);
    }
    sysc(close,1,_parent_sock);
  }

  _preclone_sigs_setup();
  _preclone_epoll_setup();
  u_memset(&_msg,0,sizeof(_msg));
}

static void _preclone_entry(void)
{
  _preclone_setup();
  k_atomic_u8_set(_preclones_states+_slot,PRECLONE_AVAILABLE);
  _preclone_loop();
}
//******************************************************************************

static void _clone_spawn(void)
{
  if(_ps_n<_ps_max){
    k_l r=sysc(clone,5,K_SIGCHLD,0,0,0,0);
    if(K_ISERR(r)){
      PERR("FATAL:error cloning %ld\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
    if(r==0) goto clone_entry_setup;
 
    ++_ps_n;
  }
  return;

clone_entry_setup:
  sysc(close,1,0);//for the connexion socket
  sysc(close,1,1);//for the connexion socket
  //keep stderr

  sysc(close,1,_sigs_fd);//don't care of parent signal
  sysc(close,1,_epfd);//don't use parent epoll

  _clone_entry();
  return;
}

static void _preclone_ctl_socket(k_i s[2])
{
  k_l r=sysc(socketpair,4,K_AF_UNIX,K_SOCK_DGRAM,0,s);
  if(K_ISERR(r)){
    PERR("FATAL:error creating preclone socket pair (%ld)\n",r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }
}

static k_i _preclone_spawn(k_i slot)
{
  k_atomic_u8_set(_preclones_states+slot,PRECLONE_BUSY);

  k_i s[2];
  _preclone_ctl_socket(s);

  k_l r=sysc(clone,5,K_SIGCHLD,0,0,0,0);
  if(K_ISERR(r)){
    PERR("FATAL:error precloning process slot %d with error %ld\n",slot,r);
    return -1;
  }
  if(r==0) goto preclone_entry_setup;
 
  _preclones[slot].s=s[1];
  sysc(close,1,s[0]); 
  _preclones[slot].pid=(k_i)r; 
  return 0;

preclone_entry_setup:
  _slot=slot;
  sysc(close,1,s[1]);//close parent unix socket
  _parent_sock=s[0];

  sysc(close,1,0);//for the connexion socket
  sysc(close,1,1);//for the connexion socket
  //keep stderr

  sysc(close,1,_sigs_fd);//don't care of parent signal
  sysc(close,1,_epfd);//don't use parent epoll

  _preclone_entry();
  return -1;
}

static void _sigs_setup(void)
{//synchronous treatement of signals with signalfd
  //cannot change SIGKILL, neither SIGSTOP
  k_ul mask=(~0);
  k_l r=sysc(rt_sigprocmask,4,K_SIG_BLOCK,&mask,0,sizeof(mask));
  if(K_ISERR(r)){
    PERR("FATAL:error blocking mostly all signals (%ld)\n",r);
    sysc(exit_group,1,-1);
  }

  mask=SIGBIT(K_SIGTERM)|SIGBIT(K_SIGCHLD);
  _sigs_fd=(k_i)sysc(signalfd4,4,-1,&mask,sizeof(mask),K_SFD_NONBLOCK);
  if(K_ISERR(_sigs_fd)){
    PERR("FATAL:error setting up fd for signals (%d)\n",_sigs_fd);
    sysc(exit_group,1,-1);
  }
}

static void _mem_allocate(void)
{
  if(!_preclones_n) return;//nothing to allocate

  k_l r=sysc(mmap,6,0,
                    (_preclones_n)*sizeof(&_preclones),
                    K_PROT_READ|K_PROT_WRITE,
                    K_MAP_PRIVATE|K_MAP_ANONYMOUS|K_MAP_POPULATE,0,0);
  if(K_ISERR(r)){
    PERR("FATAL:unable to allocate children infos memory:%ld\n",r);
    sysc(exit_group,1,-1);
  }
  _preclones=(struct preclone *)r;

  r=sysc(mmap,6,0,
                (_preclones_n)*sizeof(&_preclones_states),
                K_PROT_READ|K_PROT_WRITE,
                K_MAP_SHARED|K_MAP_ANONYMOUS|K_MAP_POPULATE,0,0);
  if(K_ISERR(r)){
    PERR("FATAL:unable to allocate children shared memory:%ld\n",r);
    sysc(exit_group,1,-1);
  }
  _preclones_states=(k_atomic_u8_t *)r;
  for(k_i slot=0;slot<_preclones_n;++slot)
    k_atomic_u8_set(_preclones_states+slot,PRECLONE_AVAILABLE);
}

static void _serv_sock_create(void)
{
  //TCP on IPv4... erk!
  _serv_sock=(k_i)sysc(socket,3,K_PF_INET,K_SOCK_STREAM|K_SOCK_NONBLOCK,0);
  if(K_ISERR(_serv_sock)){
    PERR("FATAL:unable to create the server listening socket:%d\n",_serv_sock);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }

  k_i bool_true=1;
  k_l r=sysc(setsockopt,5,_serv_sock,K_SOL_SOCKET,K_SO_REUSEADDR,&bool_true,
                          sizeof(bool_true));
  if(K_ISERR(r)){
    PERR("FATAL:unable to turn on socket rebinding option:%ld\n",r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }

  r=sysc(bind,3,_serv_sock,&_serv_addr,sizeof(_serv_addr));
  if(K_ISERR(r)){
    PERR("FATAL:unable to bind address/port on server listening socket:%ld\n",r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }

  r=sysc(listen,2,_serv_sock,0);
  if(K_ISERR(r)){
    PERR("FATAL:unable to flag server listening socket:%ld\n",r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }
}

static void _epoll_sigs_setup(void)
{
  _epfd=(k_i)sysc(epoll_create1,1,0);
  if(K_ISERR(_epfd)){
    PERR("FATAL:unable to create epoll fd (%d)\n",_epfd);
    sysc(exit_group,1,-1);
  }
  struct k_epoll_event ep_evt;
  u_memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=K_EPOLLET|K_EPOLLIN;
  ep_evt.data.fd=_sigs_fd;
  k_l r=sysc(epoll_ctl,4,_epfd,K_EPOLL_CTL_ADD,_sigs_fd,&ep_evt);
  if(K_ISERR(r)){
    PERR("FATAL:unable to register signal fd to epoll (%ld)\n",r);
    sysc(exit_group,1,-1);
  }
}

static void _epoll_serv_sock_setup(void)
{
  struct k_epoll_event ep_evt;
  u_memset(&ep_evt,0,sizeof(ep_evt));
  ep_evt.events=K_EPOLLIN|K_EPOLLPRI;
  ep_evt.data.fd=_serv_sock;
  k_l r=sysc(epoll_ctl,4,_epfd,K_EPOLL_CTL_ADD,_serv_sock,&ep_evt);
  if(K_ISERR(r)){
    PERR("FATAL:unable to register server socket to epoll (%ld)\n",r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,-1);
  }
}

static void _preclones_spawn(void)
{
  for(k_i slot=0;slot<_preclones_n;++slot)
    if(_preclone_spawn(slot)<0){
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
}

static void _setup(void)
{
  _mem_allocate();
  _sigs_setup();
  _epoll_sigs_setup();
  _preclones_spawn();
  //from there, do not forget to kill the children (configured as no zombie)
  _serv_sock_create();
  _epoll_serv_sock_setup();
}

static void _cmdline_options_allow_parse(k_u8 *list)
{
  k_u8 *start=list;
  while(*start){
    k_u8 *end=start;
    while(*end&&*end!=',') ++end;
    if(!u_a_strict_2ipv4_blk((k_u32*)list+_allow_n,start,end-1)){
      PERR("FATAL:unable to convert IPv4 address from ascii to kernel format"
           " (address n:%d)\n",_allow_n+1);
      sysc(exit_group,1,-1);
    }
    ++_allow_n;
    if(!*end) break;
    start=end+1;
  }
}

static void _cmdline_options(k_i argc,k_u8 *argv[])
{
  #define OPTION(string) u_a_strncmp((k_u8*)string,argv[i],sizeof(string)-1)==0
  #define CHECK_ARG if(argc<=i+1){_usage();sysc(exit_group,1,0);}

  for(k_i i=1;i<argc;i+=2){
    if(OPTION("-inet")){
      CHECK_ARG

      k_ul len=0;
      if(!u_a_strnlen(&len,argv[i+1],sizeof("xxx.xxx.xxx.xxx")-1)){
        PERR("FATAL:invalid listening IPv4 address(size):%s\n",argv[i+1]);
        sysc(exit_group,1,-1);
      }
      if(!u_a_strict_2ipv4_blk((k_u32*)&_serv_addr.sin_addr,argv[i+1],
                               argv[i+1]+len-1)){
        PERR("FATAL:invalid listening IPv4 address(format):%s\n",argv[i+1]);
        sysc(exit_group,1,-1);
      }
    }else if(OPTION("-port")){
      CHECK_ARG

      if(!u_a_strict_dec2u16(&_serv_addr.sin_port,argv[i+1])){
        PERR("FATAL:failed network port conversion:%s\n",argv[i+1]);
        sysc(exit_group,1,-1);
      }
      _serv_addr.sin_port=u_cpu2be16(_serv_addr.sin_port);
    }else if(OPTION("-psmax")){
      CHECK_ARG

      if(!u_a_strict_dec2us(&_ps_max,argv[i+1])){
        PERR("FATAL:failed maximum number of non precloned processes"
             " conversion:%s\n",argv[i+1]);
        sysc(exit_group,1,-1);
      }
    }else if(OPTION("-preclones")){
      CHECK_ARG

      if(!u_a_strict_dec2us(&_preclones_n,argv[i+1])){
        PERR("FATAL:failed number of precloned processes conversion:%s\n",
             argv[i+1]);
        sysc(exit_group,1,-1);
      }
    }else if(OPTION("-allows")){
      CHECK_ARG

      _cmdline_options_allow_parse(argv[i+1]);
      _allows=(k_u32*)argv[i+1];
    }else if(OPTION("-runs")){
      CHECK_ARG

      if(!u_a_strict_dec2us(&_runs,argv[i+1])){
        PERR("FATAL:failed number of runs conversion:%s\n",argv[i+1]);
        sysc(exit_group,1,-1);
      }
    }else{//"-help"
      _usage();
      sysc(exit_group,1,0);
    }
  }
}

static void _sigchld(void)
{
  while(1){
    k_l r;
    do{
      r=sysc(wait4,4,-1,0,K_WNOHANG|K_WALL,0);
    }while(r==-K_EINTR);
    //ECHILD seems to means not even 1 child to wait for
    if(r!=-K_ECHILD&&K_ISERR(r)){
      PERR("FATAL:error waiting on dead children (%ld)\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
    if(r==0||r==-K_ECHILD) break;

    k_i slot;
    for(slot=0;slot<_preclones_n;++slot) if(_preclones[slot].pid==(k_i)r) break;

    if(slot<_preclones_n){
      sysc(close,1,_preclones[slot].s);
      if(_preclone_spawn(slot)<0){
        sysc(kill,2,0,K_SIGTERM);
        sysc(exit_group,1,-1);
      }
    }else _ps_n--;//a not precloned process died
  }
}

static void _sigs_consume(void)
{
  static struct k_signalfd_siginfo info;

  while(1){
    k_l r;
    do{
      u_memset(&info,0,sizeof(info));
      r=sysc(read,3,_sigs_fd,&info,sizeof(info));
    }while(r==-K_EINTR);
    if(r!=-K_EAGAIN&&((K_ISERR(r)||(r>0&&r!=sizeof(info))))){
      PERR("FATAL:error consuming signals (%ld)\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
    if(r==0||r==-K_EAGAIN) break;

    switch(info.ssi_signo){
    case K_SIGCHLD:
      _sigchld();
      break;
    case K_SIGTERM:
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,0);
    }
  }
}

static void _cnx_send(k_i slot)
{
  _cnx_sendrecv_setup();
  *((k_i*)(K_CMSG_DATA(K_CMSG_FIRSTHDR(&_msg))))=_cnx_sock;

  k_l r=sysc(sendmsg,3,_preclones[slot].s,&_msg,0);
  if(K_ISERR(r)){
    PERR("FATAL:error sending connexion socket to preclone %d (%ld)\n",slot,r);
    sysc(kill,2,0,K_SIGTERM);
    sysc(exit_group,1,0);
  }
}

static void _cnx_dispatch(void)
{
  k_i slot;
  for(slot=0;slot<_preclones_n;++slot)
    if(k_atomic_u8_read(_preclones_states+slot)==PRECLONE_AVAILABLE) break;

  if(slot<_preclones_n){
    k_atomic_u8_set(_preclones_states+slot,PRECLONE_BUSY);
    _cnx_send(slot);
  }else _clone_spawn();
  sysc(close,1,_cnx_sock);
}

static k_ut _peer_is_allowed(k_u32 ipv4)
{
  for(k_i i=0;i<_allow_n;++i) if(_allows[i]==ipv4) return 1;
  return 0;
}

static void _cnxs_consume(void)
{
  while(1){
    k_l r;
    struct k_sockaddr_in peer;
    int peer_len=sizeof(peer);
    do{
      r=sysc(accept,3,_serv_sock,&peer,&peer_len);
    }while(r==-K_EINTR||r==-K_ECONNABORTED);

    if(r!=-K_EAGAIN&&K_ISERR(r)){//error
      PERR("FATAL:error accepting connexion:%ld\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
    if(peer_len!=sizeof(peer)){
      PERR("FATAL:error accepting connexion(socket address size):%ld\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }

    if(r==-K_EAGAIN) break;//no more connexion pending

    _cnx_sock=(k_i)r;
    if(_allow_n){//allow filter is unabled if at least one address was provided
      if(_peer_is_allowed(peer.sin_addr.s_addr)) _cnx_dispatch();
      else sysc(close,1,_cnx_sock);
    }else _cnx_dispatch();
  }
}

static void _loop(void)
{
  while(1){
    static struct k_epoll_event evts[2];//_sigs_fd and _serv_sock
    k_l r;
    do{
      u_memset(evts,0,sizeof(evts));
      r=sysc(epoll_wait,4,_epfd,evts,2,-1);
    }while(r==-K_EINTR);
    if(K_ISERR(r)){
      PERR("FATAL:error epolling fds (%ld)\n",r);
      sysc(kill,2,0,K_SIGTERM);
      sysc(exit_group,1,-1);
    }
    for(k_l i=0;i<r;++i)
      if(evts[i].data.fd==_sigs_fd){
        if(evts[i].events&K_EPOLLIN) _sigs_consume();
        else{
          PERR("FATAL:something wrong with signal fd epolling n=%ld"
               " events=%u\n",i,evts[i].events);
          sysc(kill,2,0,K_SIGTERM);
          sysc(exit_group,1,-1);
        }
      }else if(evts[i].data.fd==_serv_sock){
        if(evts[i].events&(K_EPOLLERR|K_EPOLLHUP|K_EPOLLPRI)){
          PERR("FATAL:something wrong with server socket epolling n=%ld"
               " events=%u\n",i,evts[i].events);
          sysc(kill,2,0,K_SIGTERM);
          sysc(exit_group,1,-1);
        }else if(evts[i].events&K_EPOLLIN){
          _cnxs_consume();
        }else{
          PERR("FATAL:unknown epolling event on server socket n=%ld"
               " events=%u\n",i,evts[i].events);
          sysc(kill,2,0,K_SIGTERM);
          sysc(exit_group,1,-1);
        }
      }
  }
}

//*****************************************************************************
//XXX:may do the daemonic stuff if really we need it:logger instead of tty,
//    own session...
void start(k_i argc,k_u8 **argv)
{
  k_u8 dprintf_buf[_DPRINTF_BUF_SZ];
  _dprintf_buf=&dprintf_buf[0];

  _cmdline_options(argc,argv);
  _setup();
  _loop();
}
