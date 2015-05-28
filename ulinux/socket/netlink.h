#ifndef ULINUX_SOCKET_NETLINK_H
#define ULINUX_SOCKET_NETLINK_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_NETLINK_ROUTE          0/*routing/device hook*/
#define ULINUX_NETLINK_UNUSED         1/*unused number*/
#define ULINUX_NETLINK_USERSOCK       2/*reserved for user mode socket protos*/
#define ULINUX_NETLINK_FIREWALL       3/*firewalling hook*/
#define ULINUX_NETLINK_INET_DIAG      4/*inet socket monitoring*/
#define ULINUX_NETLINK_NFLOG          5/*netfilter/iptables ulog*/
#define ULINUX_NETLINK_XFRM           6/*ipsec*/
#define ULINUX_NETLINK_SELINUX        7/*selinux event notifications*/
#define ULINUX_NETLINK_ISCSI          8/*open-iscsi*/
#define ULINUX_NETLINK_AUDIT          9/*auditing */
#define ULINUX_NETLINK_FIB_LOOKUP     10	
#define ULINUX_NETLINK_CONNECTOR      11
#define ULINUX_NETLINK_NETFILTER      12/*netfilter subsystem*/
#define ULINUX_NETLINK_IP6_FW         13
#define ULINUX_NETLINK_DNRTMSG        14/*decnet routing messages*/
#define ULINUX_NETLINK_KOBJECT_UEVENT 15/*kernel messages to userspace*/
#define ULINUX_NETLINK_GENERIC        16
/*leave room for netlink_dm (dm events)*/
#define ULINUX_NETLINK_SCSITRANSPORT  18/*scsi transports*/
#define ULINUX_NETLINK_ECRYPTFS       19
#define ULINUX_NETLINK_RDMA           20
#define ULINUX_MAX_LINKS              32		

struct ulinux_sockaddr_nl{
  ulinux_us nl_family;/*AF_NETLINK*/
  ulinux_us nl_pad;/*zero*/
  ulinux_u32 nl_pid;/**port* id*/
  ulinux_u32	nl_groups;/*multicast groups mask*/
  /*padding on ULINUX_SOCK_ADDR_SZ in 0*/
};

struct ulinux_nl_msg_hdr{
  ulinux_u32 len;/*length of message including header*/
  ulinux_u16 type;/*message content*/
  ulinux_u16 flgs;/*additional flags*/
  ulinux_u32 seq;/*sequence number*/
  ulinux_u32	pid;/*sending process port id*/
};

/*flags values*/
#define ULINUX_NLM_F_REQUEST   1/*it is request message*/
#define ULINUX_NLM_F_MULTI     2/*multipart message, terminated by nlmsg_done*/
#define ULINUX_NLM_F_ACK       4/*reply with ack, with zero or error code*/
#define ULINUX_NLM_F_ECHO      8/*echo this request*/
#define ULINUX_NLM_F_DUMP_INTR 16/*dump was inconsistent due to sequence
                                   change*/

/*modifiers to get request*/
#define ULINUX_NLM_F_ROOT   0x100/*specify tree root*/
#define ULINUX_NLM_F_MATCH  0x200/*return all matching*/
#define ULINUX_NLM_F_ATOMIC 0x400/*atomic get*/
#define ULINUX_NLM_F_DUMP   (ULINUX_NLM_F_ROOT|ULINUXNLM_F_MATCH)

/*modifiers to new request*/
#define ULINUX_NLM_F_REPLACE 0x100/*override existing*/
#define ULINUX_NLM_F_EXCL    0x200/*do not touch, if it exists*/
#define ULINUX_NLM_F_CREATE  0x400/*create, if it does not exist*/
#define ULINUX_NLM_F_APPEND  0x800/*add to end of list*/

#define ULINUX_NLMSG_ALIGNTO 4U
#define ULINUX_NLMSG_ALIGN(len) (((len)+ULINUX_NLMSG_ALIGNTO-1)\
&~(ULINUX_NLMSG_ALIGNTO-1))
#define ULINUX_NLMSG_HDRLEN ((ulinux_si)ULINUX_NLMSG_ALIGN(\
sizeof(struct ulinux_nl_msg_hdr)))
#define ULINUX_NLMSG_LENGTH(len) ((len)+ULINUX_NLMSG_ALIGN(ULINUX_NLMSG_HDRLEN))
#define ULINUX_NLMSG_SPACE(len) ULINUX_NLMSG_ALIGN(ULINUX_NLMSG_LENGTH(len))
#define ULINUX_NLMSG_DATA(nlh) ((void*)(((ulinux_uc*)nlh)\
+ULINUX_NLMSG_LENGTH(0)))
#define ULINUX_NLMSG_NEXT(nlh,len) ((len)-=ULINUX_NLMSG_ALIGN((nlh)->len),\
                              (struct ulinux_nl_msg_hdr*)(((ulinux_uc*)(nlh))\
                                               +ULINUX_NLMSG_ALIGN((nlh)->len)))
#define ULINUX_NLMSG_OK(nlh,len) \
((len)>=(ulinux_si)sizeof(struct ulinux_nl_msg_hdr)\
&&(nlh)->len>=sizeof(struct ulinux_nl_msg_hdr)\
&&(nlh)->len<=(len))
#define ULINUX_NLMSG_PAYLOAD(nlh,len) ((nlh)->len-ULINUX_NLMSG_SPACE((len)))

#define ULINUX_NLMSG_NOOP     0x1/*nothing*/
#define ULINUX_NLMSG_ERROR    0x2/*error*/
#define ULINUX_NLMSG_DONE     0x3/*end of a dump*/
#define ULINUX_NLMSG_OVERRUN  0x4/*data lost*/
#define ULINUX_NLMSG_MIN_TYPE 0x10/*<0x10:reserved control messages*/

struct ulinux_nl_msg_err{
  ulinux_si error;
  struct ulinux_nl_msg_hdr msg;
};
#endif
