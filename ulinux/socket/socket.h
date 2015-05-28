#ifndef ULINUX_SOCKET_SOCKET_H
#define ULINUX_SOCKET_SOCKET_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_SOCK_ADDR_SZ 16/*sizeof(struct sockaddr)*/

#define ULINUX_AF_UNSPEC     0
#define ULINUX_AF_UNIX       1/*Unix domain sockets*/
#define ULINUX_AF_LOCAL      1/*POSIX name for AF_UNIX*/
#define ULINUX_AF_INET       2/*Internet IP Protocol*/
#define ULINUX_AF_AX25       3/*Amateur Radio AX.25*/
#define ULINUX_AF_IPX        4/*Novell IPX*/
#define ULINUX_AF_APPLETALK  5/*AppleTalk DDP*/
#define ULINUX_AF_NETROM     6/*Amateur Radio NET/ROM*/
#define ULINUX_AF_BRIDGE     7/*Multiprotocol bridge*/
#define ULINUX_AF_ATMPVC     8/*ATM PVCs*/
#define ULINUX_AF_X25        9/*Reserved for X.25 project*/
#define ULINUX_AF_INET6      10/*IP version 6*/
#define ULINUX_AF_ROSE       11/*Amateur Radio X.25 PLP*/
#define ULINUX_AF_DECnet     12/*Reserved for DECnet project*/
#define ULINUX_AF_NETBEUI    13/*Reserved for 802.2LLC project*/
#define ULINUX_AF_SECURITY   14/*Security callback pseudo AF*/
#define ULINUX_AF_KEY        15/*PF_KEY key management API*/
#define ULINUX_AF_NETLINK    16
#define ULINUX_AF_ROUTE      ULINUX_AF_NETLINK/*Alias to emulate 4.4BSD*/
#define ULINUX_AF_PACKET     17/*Packet family*/
#define ULINUX_AF_ASH        18/*Ash*/
#define ULINUX_AF_ECONET     19/*Acorn Econet*/
#define ULINUX_AF_ATMSVC     20/*ATM SVCs*/
#define ULINUX_AF_RDS        21/*RDS sockets*/
#define ULINUX_AF_SNA        22/*Linux SNA Project (nutters!)*/
#define ULINUX_AF_IRDA       23/*IRDA sockets*/
#define ULINUX_AF_PPPOX      24/*PPPoX sockets*/
#define ULINUX_AF_WANPIPE    25/*Wanpipe API Sockets*/
#define ULINUX_AF_LLC        26/*Linux LLC*/
#define ULINUX_AF_CAN        29/*Controller Area Network*/
#define ULINUX_AF_TIPC       30/*TIPC sockets*/
#define ULINUX_AF_BLUETOOTH  31/*Bluetooth sockets*/
#define ULINUX_AF_IUCV       32/*IUCV sockets*/
#define ULINUX_AF_RXRPC      33/*RxRPC sockets*/
#define ULINUX_AF_ISDN       34/*mISDN sockets*/
#define ULINUX_AF_PHONET     35/*Phonet sockets*/
#define ULINUX_AF_IEEE802154 36/*IEEE802154 sockets*/
#define ULINUX_AF_CAIF       37/*CAIF sockets*/
#define ULINUX_AF_MAX        38/*For now..*/

/*protocol families, same as address families*/
#define ULINUX_PF_UNSPEC     ULINUX_AF_UNSPEC
#define ULINUX_PF_UNIX       ULINUX_AF_UNIX
#define ULINUX_PF_LOCAL      ULINUX_AF_LOCAL
#define ULINUX_PF_INET       ULINUX_AF_INET
#define ULINUX_PF_AX25       ULINUX_AF_AX25
#define ULINUX_PF_IPX        ULINUX_AF_IPX
#define ULINUX_PF_APPLETALK  ULINUX_AF_APPLETALK
#define ULINUX_PF_NETROM     ULINUX_AF_NETROM
#define ULINUX_PF_BRIDGE     ULINUX_AF_BRIDGE
#define ULINUX_PF_ATMPVC     ULINUX_AF_ATMPVC
#define ULINUX_PF_X25        ULINUX_AF_X25
#define ULINUX_PF_INET6      ULINUX_AF_INET6
#define ULINUX_PF_ROSE       ULINUX_AF_ROSE
#define ULINUX_PF_DECnet     ULINUX_AF_DECnet
#define ULINUX_PF_NETBEUI    ULINUX_AF_NETBEUI
#define ULINUX_PF_SECURITY   ULINUX_AF_SECURITY
#define ULINUX_PF_KEY        ULINUX_AF_KEY
#define ULINUX_PF_NETLINK    ULINUX_AF_NETLINK
#define ULINUX_PF_ROUTE      ULINUX_AF_ROUTE
#define ULINUX_PF_PACKET     ULINUX_AF_PACKET
#define ULINUX_PF_ASH        ULINUX_AF_ASH
#define ULINUX_PF_ECONET     ULINUX_AF_ECONET
#define ULINUX_PF_ATMSVC     ULINUX_AF_ATMSVC
#define ULINUX_PF_RDS        ULINUX_AF_RDS
#define ULINUX_PF_SNA        ULINUX_AF_SNA
#define ULINUX_PF_IRDA       ULINUX_AF_IRDA
#define ULINUX_PF_PPPOX      ULINUX_AF_PPPOX
#define ULINUX_PF_WANPIPE    ULINUX_AF_WANPIPE
#define ULINUX_PF_LLC        ULINUX_AF_LLC
#define ULINUX_PF_CAN        ULINUX_AF_CAN
#define ULINUX_PF_TIPC       ULINUX_AF_TIPC
#define ULINUX_PF_BLUETOOTH  ULINUX_AF_BLUETOOTH
#define ULINUX_PF_IUCV       ULINUX_AF_IUCV
#define ULINUX_PF_RXRPC      ULINUX_AF_RXRPC
#define ULINUX_PF_ISDN       ULINUX_AF_ISDN
#define ULINUX_PF_PHONET     ULINUX_AF_PHONET
#define ULINUX_PF_IEEE802154 ULINUX_AF_IEEE802154
#define ULINUX_PF_CAIF       ULINUX_AF_CAIF
#define ULINUX_PF_MAX        ULINUX_AF_MAX

enum ulinux_sock_type{
 ULINUX_SOCK_STREAM=   1,
 ULINUX_SOCK_DGRAM=    2,
 ULINUX_SOCK_RAW=      3,
 ULINUX_SOCK_RDM=      4,
 ULINUX_SOCK_SEQPACKET=5,
 ULINUX_SOCK_DCCP=     6,
 ULINUX_SOCK_PACKET=   10,
};

#define ULINUX_SOCK_NONBLOCK ULINUX_O_NONBLOCK

/*----------------------------------------------------------------------------*/
/*options*/
#define ULINUX_SOL_SOCKET	1/*sol is *so*cket *l*evel for setsockopt(2)*/

#define ULINUX_SO_DEBUG                          1
#define ULINUX_SO_REUSEADDR                      2
#define ULINUX_SO_TYPE                           3
#define ULINUX_SO_ERROR                          4
#define ULINUX_SO_DONTROUTE                      5
#define ULINUX_SO_BROADCAST                      6
#define ULINUX_SO_SNDBUF                         7
#define ULINUX_SO_RCVBUF                         8
#define ULINUX_SO_SNDBUFFORCE                   32
#define ULINUX_SO_RCVBUFFORCE                   33
#define ULINUX_SO_KEEPALIVE                      9
#define ULINUX_SO_OOBINLINE                     10
#define ULINUX_SO_NO_CHECK                      11
#define ULINUX_SO_PRIORITY                      12
#define ULINUX_SO_LINGER                        13
#define ULINUX_SO_BSDCOMPAT                     14
/*to add :#define SO_REUSEPORT 15*/
#define ULINUX_SO_PASSCRED                      16
#define ULINUX_SO_PEERCRED                      17
#define ULINUX_SO_RCVLOWAT                      18
#define ULINUX_SO_SNDLOWAT                      19
#define ULINUX_SO_RCVTIMEO                      20
#define ULINUX_SO_SNDTIMEO                      21

/*security levels - as per NRL IPv6 - don't actually do anything*/
#define ULINUX_SO_SECURITY_AUTHENTICATION       22
#define ULINUX_SO_SECURITY_ENCRYPTION_TRANSPORT 23
#define ULINUX_SO_SECURITY_ENCRYPTION_NETWORK   24

#define ULINUX_SO_BINDTODEVICE                  25

/*socket filtering*/
#define ULINUX_SO_ATTACH_FILTER                 26
#define ULINUX_SO_DETACH_FILTER                 27

#define ULINUX_SO_PEERNAME                      28
#define ULINUX_SO_TIMESTAMP                     29
#define ULINUX_SCM_TIMESTAMP                    ULINUX_SO_TIMESTAMP

#define ULINUX_SO_ACCEPTCONN                    30

#define ULINUX_SO_PEERSEC                       31
#define ULINUX_SO_PASSSEC                       34
#define ULINUX_SO_TIMESTAMPNS                   35
#define ULINUX_SCM_TIMESTAMPNS                  ULINUX_SO_TIMESTAMPNS

#define ULINUX_SO_MARK                          36

#define ULINUX_SO_TIMESTAMPING                  37
#define ULINUX_SCM_TIMESTAMPING                 ULINUX_SO_TIMESTAMPING

#define ULINUX_SO_PROTOCOL                      38
#define ULINUX_SO_DOMAIN                        39

#define ULINUX_SO_RXQ_OVFL                      40
#define ULINUX_SO_CLOEXEC                       02000000/*= K_O_CLOEXEC*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*socketcall multiplexer*/
#define ULINUX_SYS_SOCKET      1
#define ULINUX_SYS_BIND        2
#define ULINUX_SYS_CONNECT     3
#define ULINUX_SYS_LISTEN      4
#define ULINUX_SYS_ACCEPT      5
#define ULINUX_SYS_GETSOCKNAME 6
#define ULINUX_SYS_GETPEERNAME 7
#define ULINUX_SYS_SOCKETPAIR  8
#define ULINUX_SYS_SEND        9
#define ULINUX_SYS_RECV        10
#define ULINUX_SYS_SENDTO      11
#define ULINUX_SYS_RECVFROM    12
#define ULINUX_SYS_SHUTDOWN    13
#define ULINUX_SYS_SETSOCKOPT  14
#define ULINUX_SYS_GETSOCKOPT  15
#define ULINUX_SYS_SENDMSG     16
#define ULINUX_SYS_RECVMSG     17
#define ULINUX_SYS_ACCEPT4     18
#define ULINUX_SYS_RECVMMSG    19
#define ULINUX_SYS_SENDMMSG    20
#endif
