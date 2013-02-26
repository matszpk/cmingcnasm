#ifndef ULINUX_SOCKET_SOCKET_H
#define ULINUX_SOCKET_SOCKET_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define __SOCK_ADDR_SZ__ 16//sizeof(struct sockaddr)

#define K_AF_UNSPEC     0
#define K_AF_UNIX       1//Unix domain sockets
#define K_AF_LOCAL      1//POSIX name for AF_UNIX
#define K_AF_INET       2//Internet IP Protocol
#define K_AF_AX25       3//Amateur Radio AX.25
#define K_AF_IPX        4//Novell IPX
#define K_AF_APPLETALK  5//AppleTalk DDP
#define K_AF_NETROM     6//Amateur Radio NET/ROM
#define K_AF_BRIDGE     7//Multiprotocol bridge
#define K_AF_ATMPVC     8//ATM PVCs
#define K_AF_X25        9//Reserved for X.25 project
#define K_AF_INET6      10//IP version 6
#define K_AF_ROSE       11//Amateur Radio X.25 PLP
#define K_AF_DECnet     12//Reserved for DECnet project
#define K_AF_NETBEUI    13//Reserved for 802.2LLC project
#define K_AF_SECURITY   14//Security callback pseudo AF
#define K_AF_KEY        15//PF_KEY key management API
#define K_AF_NETLINK    16
#define K_AF_ROUTE      K_AF_NETLINK//Alias to emulate 4.4BSD
#define K_AF_PACKET     17//Packet family
#define K_AF_ASH        18//Ash
#define K_AF_ECONET     19//Acorn Econet
#define K_AF_ATMSVC     20//ATM SVCs
#define K_AF_RDS        21//RDS sockets
#define K_AF_SNA        22//Linux SNA Project (nutters!)
#define K_AF_IRDA       23//IRDA sockets
#define K_AF_PPPOX      24//PPPoX sockets
#define K_AF_WANPIPE    25//Wanpipe API Sockets
#define K_AF_LLC        26//Linux LLC
#define K_AF_CAN        29//Controller Area Network
#define K_AF_TIPC       30//TIPC sockets
#define K_AF_BLUETOOTH  31//Bluetooth sockets
#define K_AF_IUCV       32//IUCV sockets
#define K_AF_RXRPC      33//RxRPC sockets
#define K_AF_ISDN       34//mISDN sockets
#define K_AF_PHONET     35//Phonet sockets
#define K_AF_IEEE802154 36//IEEE802154 sockets
#define K_AF_CAIF       37//CAIF sockets
#define K_AF_MAX        38//For now..

//protocol families, same as address families
#define K_PF_UNSPEC     K_AF_UNSPEC
#define K_PF_UNIX       K_AF_UNIX
#define K_PF_LOCAL      K_AF_LOCAL
#define K_PF_INET       K_AF_INET
#define K_PF_AX25       K_AF_AX25
#define K_PF_IPX        K_AF_IPX
#define K_PF_APPLETALK  K_AF_APPLETALK
#define K_PF_NETROM     K_AF_NETROM
#define K_PF_BRIDGE     K_AF_BRIDGE
#define K_PF_ATMPVC     K_AF_ATMPVC
#define K_PF_X25        K_AF_X25
#define K_PF_INET6      K_AF_INET6
#define K_PF_ROSE       K_AF_ROSE
#define K_PF_DECnet     K_AF_DECnet
#define K_PF_NETBEUI    K_AF_NETBEUI
#define K_PF_SECURITY   K_AF_SECURITY
#define K_PF_KEY        K_AF_KEY
#define K_PF_NETLINK    K_AF_NETLINK
#define K_PF_ROUTE      K_AF_ROUTE
#define K_PF_PACKET     K_AF_PACKET
#define K_PF_ASH        K_AF_ASH
#define K_PF_ECONET     K_AF_ECONET
#define K_PF_ATMSVC     K_AF_ATMSVC
#define K_PF_RDS        K_AF_RDS
#define K_PF_SNA        K_AF_SNA
#define K_PF_IRDA       K_AF_IRDA
#define K_PF_PPPOX      K_AF_PPPOX
#define K_PF_WANPIPE    K_AF_WANPIPE
#define K_PF_LLC        K_AF_LLC
#define K_PF_CAN        K_AF_CAN
#define K_PF_TIPC       K_AF_TIPC
#define K_PF_BLUETOOTH  K_AF_BLUETOOTH
#define K_PF_IUCV       K_AF_IUCV
#define K_PF_RXRPC      K_AF_RXRPC
#define K_PF_ISDN       K_AF_ISDN
#define K_PF_PHONET     K_AF_PHONET
#define K_PF_IEEE802154 K_AF_IEEE802154
#define K_PF_CAIF       K_AF_CAIF
#define K_PF_MAX        K_AF_MAX

enum k_sock_type{
 K_SOCK_STREAM=   1,
 K_SOCK_DGRAM=    2,
 K_SOCK_RAW=      3,
 K_SOCK_RDM=      4,
 K_SOCK_SEQPACKET=5,
 K_SOCK_DCCP=     6,
 K_SOCK_PACKET=   10,
};

#define K_SOCK_NONBLOCK K_O_NONBLOCK

//------------------------------------------------------------------------------
//options
//------------------------------------------------------------------------------
#define K_SOL_SOCKET	1//sol is *so*cket *l*evel for setsockopt(2)

#define K_SO_DEBUG                          1
#define K_SO_REUSEADDR                      2
#define K_SO_TYPE                           3
#define K_SO_ERROR                          4
#define K_SO_DONTROUTE                      5
#define K_SO_BROADCAST                      6
#define K_SO_SNDBUF                         7
#define K_SO_RCVBUF                         8
#define K_SO_SNDBUFFORCE                   32
#define K_SO_RCVBUFFORCE                   33
#define K_SO_KEEPALIVE                      9
#define K_SO_OOBINLINE                     10
#define K_SO_NO_CHECK                      11
#define K_SO_PRIORITY                      12
#define K_SO_LINGER                        13
#define K_SO_BSDCOMPAT                     14
//to add :#define SO_REUSEPORT 15
#define K_SO_PASSCRED                      16
#define K_SO_PEERCRED                      17
#define K_SO_RCVLOWAT                      18
#define K_SO_SNDLOWAT                      19
#define K_SO_RCVTIMEO                      20
#define K_SO_SNDTIMEO                      21

//security levels - as per NRL IPv6 - don't actually do anything
#define K_SO_SECURITY_AUTHENTICATION       22
#define K_SO_SECURITY_ENCRYPTION_TRANSPORT 23
#define K_SO_SECURITY_ENCRYPTION_NETWORK   24

#define K_SO_BINDTODEVICE                  25

//socket filtering
#define K_SO_ATTACH_FILTER                 26
#define K_SO_DETACH_FILTER                 27

#define K_SO_PEERNAME                      28
#define K_SO_TIMESTAMP                     29
#define K_SCM_TIMESTAMP                    K_SO_TIMESTAMP

#define K_SO_ACCEPTCONN                    30

#define K_SO_PEERSEC                       31
#define K_SO_PASSSEC                       34
#define K_SO_TIMESTAMPNS                   35
#define K_SCM_TIMESTAMPNS                  K_SO_TIMESTAMPNS

#define K_SO_MARK                          36

#define K_SO_TIMESTAMPING                  37
#define K_SCM_TIMESTAMPING                 K_SO_TIMESTAMPING

#define K_SO_PROTOCOL                      38
#define K_SO_DOMAIN                        39

#define K_SO_RXQ_OVFL                      40
#define K_SO_CLOEXEC                       02000000//same than K_O_CLOEXEC
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//socketcall multiplexer
//------------------------------------------------------------------------------
#define K_SYS_SOCKET      1
#define K_SYS_BIND        2
#define K_SYS_CONNECT     3
#define K_SYS_LISTEN      4
#define K_SYS_ACCEPT      5
#define K_SYS_GETSOCKNAME 6
#define K_SYS_GETPEERNAME 7
#define K_SYS_SOCKETPAIR  8
#define K_SYS_SEND        9
#define K_SYS_RECV        10
#define K_SYS_SENDTO      11
#define K_SYS_RECVFROM    12
#define K_SYS_SHUTDOWN    13
#define K_SYS_SETSOCKOPT  14
#define K_SYS_GETSOCKOPT  15
#define K_SYS_SENDMSG     16
#define K_SYS_RECVMSG     17
#define K_SYS_ACCEPT4     18
#define K_SYS_RECVMMSG    19
#define K_SYS_SENDMMSG    20
#endif
