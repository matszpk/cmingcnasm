#ifndef ULINUX_SOCKET_NETLINK_H
#define ULINUX_SOCKET_NETLINK_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_NETLINK_ROUTE          0//routing/device hook
#define K_NETLINK_UNUSED         1//unused number
#define K_NETLINK_USERSOCK       2//reserved for user mode socket protocols
#define K_NETLINK_FIREWALL       3//firewalling hook
#define K_NETLINK_INET_DIAG      4//inet socket monitoring
#define K_NETLINK_NFLOG          5//netfilter/iptables ulog
#define K_NETLINK_XFRM           6//ipsec
#define K_NETLINK_SELINUX        7//selinux event notifications
#define K_NETLINK_ISCSI          8//open-iscsi
#define K_NETLINK_AUDIT          9//auditing 
#define K_NETLINK_FIB_LOOKUP     10	
#define K_NETLINK_CONNECTOR      11
#define K_NETLINK_NETFILTER      12//netfilter subsystem
#define K_NETLINK_IP6_FW         13
#define K_NETLINK_DNRTMSG        14//decnet routing messages
#define K_NETLINK_KOBJECT_UEVENT 15//kernel messages to userspace
#define K_NETLINK_GENERIC        16
//leave room for netlink_dm (dm events)
#define K_NETLINK_SCSITRANSPORT  18//scsi transports
#define K_NETLINK_ECRYPTFS       19
#define K_NETLINK_RDMA           20
#define K_MAX_LINKS              32		

struct k_sockaddr_nl{
  k_us nl_family;//AF_NETLINK
  k_us nl_pad;//zero
  k_u32 nl_pid;//*port* id
  k_u32	nl_groups;//multicast groups mask
  //padding on __SOCK_ADDR_SZ__ in 0
};

struct k_nl_msg_hdr{
  k_u32 len;//length of message including header
  k_u16 type;//message content
  k_u16 flgs;//additional flags
  k_u32 seq;//sequence number
  k_u32	pid;//sending process port id
};

//flags values
#define K_NLM_F_REQUEST   1//it is request message
#define K_NLM_F_MULTI     2//multipart message, terminated by nlmsg_done
#define K_NLM_F_ACK       4//reply with ack, with zero or error code
#define K_NLM_F_ECHO      8//echo this request
#define K_NLM_F_DUMP_INTR 16//dump was inconsistent due to sequence change

//modifiers to get request
#define K_NLM_F_ROOT   0x100//specify tree root
#define K_NLM_F_MATCH  0x200//return all matching
#define K_NLM_F_ATOMIC 0x400//atomic get
#define K_NLM_F_DUMP   (NLM_F_ROOT|NLM_F_MATCH)

//modifiers to new request
#define K_NLM_F_REPLACE 0x100//override existing
#define K_NLM_F_EXCL    0x200//do not touch, if it exists
#define K_NLM_F_CREATE  0x400//create, if it does not exist
#define K_NLM_F_APPEND  0x800//add to end of list

#define K_NLMSG_ALIGNTO 4U
#define K_NLMSG_ALIGN(len) (((len)+K_NLMSG_ALIGNTO-1)&~(K_NLMSG_ALIGNTO-1))
#define K_NLMSG_HDRLEN ((k_i)K_NLMSG_ALIGN(sizeof(struct k_nl_msg_hdr)))
#define K_NLMSG_LENGTH(len) ((len)+K_NLMSG_ALIGN(K_NLMSG_HDRLEN))
#define K_NLMSG_SPACE(len) K_NLMSG_ALIGN(K_NLMSG_LENGTH(len))
#define K_NLMSG_DATA(nlh) ((void*)(((k_ut*)nlh)+K_NLMSG_LENGTH(0)))
#define K_NLMSG_NEXT(nlh,len) ((len)-=K_NLMSG_ALIGN((nlh)->len),\
                              (struct k_nl_msg_hdr*)(((k_ut*)(nlh))\
                                                   +K_NLMSG_ALIGN((nlh)->len)))
#define K_NLMSG_OK(nlh,len) ((len)>=(k_i)sizeof(struct k_nl_msg_hdr)\
                             &&(nlh)->len>=sizeof(struct k_nl_msg_hdr)\
                             &&(nlh)->len<=(len))
#define K_NLMSG_PAYLOAD(nlh,len) ((nlh)->len-K_NLMSG_SPACE((len)))

#define K_NLMSG_NOOP     0x1//nothing
#define K_NLMSG_ERROR    0x2//error
#define K_NLMSG_DONE     0x3//end of a dump
#define K_NLMSG_OVERRUN	 0x4//data lost
#define K_NLMSG_MIN_TYPE 0x10//<0x10:reserved control messages

struct k_nl_msg_err{
  k_i error;
  struct k_nl_msg_hdr msg;
};
#endif
