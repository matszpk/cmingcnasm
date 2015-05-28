#ifndef ULINUX_SOCKET_MSG_H
#define ULINUX_SOCKET_MSG_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
#define ULINUX_SOL_SOCKET 1

struct ulinux_io_vec{/*will probably go somewhere else*/
  void *base;/*BSD uses caddr_t (1003.1g requires void *)*/
  ulinux_sz len;/*Must be size_t (1003.1g)*/
};

struct ulinux_msg_hdr{
  void *name;/*socket name*/
  ulinux_si name_len;/*length of name*/
  struct ulinux_io_vec *iov;/*data blocks*/
  ulinux_sz iov_len;/*number of blocks*/
  void *ctl;/*per protocol magic (eg BSD file descriptor passing)*/
  ulinux_sz ctl_len;/*length of cmsg list*/
  ulinux_ui flgs;
}; 

/*the c prefix stands for access *c*ontrol message (other name ancillary data)*/
struct ulinux_cmsg_hdr{
  ulinux_sz len;/*data byte count, including hdr*/
  ulinux_si lvl;/*originating protocol*/
  ulinux_si type;/*protocol-specific type*/
};

#define __ULINUX_CMSG_NXTHDR(ctl,len,cmsg) \
__ulinux_cmsg_nxthdr((ctl),(len),(cmsg))
#define ULINUX_CMSG_NXTHDR(mhdr,cmsg) ulinux_cmsg_nxthdr((mhdr),(cmsg))

#define ULINUX_CMSG_ALIGN(len) (((len)+sizeof(ulinux_sl)-1)\
&~(sizeof(ulinux_sl)-1))

#define ULINUX_CMSG_DATA(cmsg) ((void*)((ulinux_u8*)(cmsg)\
                           +ULINUX_CMSG_ALIGN(sizeof(struct ulinux_cmsg_hdr))))
#define ULINUX_CMSG_SPACE(len) \
(ULINUX_CMSG_ALIGN(sizeof(struct ulinux_cmsg_hdr))+ULINUX_CMSG_ALIGN(len))
#define ULINUX_CMSG_LEN(len) \
(ULINUX_CMSG_ALIGN(sizeof(struct ulinux_cmsg_hdr))+(len))

#define __ULINUX_CMSG_FIRSTHDR(ctl,len) ((len)>=sizeof(struct ulinux_cmsg_hdr)\
? (struct ulinux_cmsg_hdr *)(ctl) : (struct ulinux_cmsg_hdr *)0)
#define ULINUX_CMSG_FIRSTHDR(msg) __ULINUX_CMSG_FIRSTHDR((msg)->ctl,\
                                               (msg)->ctl_len)
#define ULINUX_CMSG_OK(mhdr, cmsg) \
((cmsg)->cmsg_len>=sizeof(struct ulinux_cmsg_hdr)&&\
                               (cmsg)->cmsg_len<=(ulinux_ul)\
                              ((mhdr)->ctl_len-\
                               ((ulinux_u8*)(cmsg)-(ulinux_u8*)(mhdr)->ctl)))

static inline struct ulinux_cmsg_hdr * __ulinux_cmsg_nxthdr(void *ctl,
                                    ulinux_sz size,struct ulinux_cmsg_hdr *cmsg)
{
  struct ulinux_cmsg_hdr *ptr;

  ptr=(struct ulinux_cmsg_hdr*)(((ulinux_u8*)cmsg)
                                                 +ULINUX_CMSG_ALIGN(cmsg->len));
  if((ulinux_ul)((ulinux_u8*)(ptr+1)-(ulinux_u8*)ctl)>size)
    return (struct ulinux_cmsg_hdr *)0;
  return ptr;
}

static inline struct ulinux_cmsg_hdr *ulinux_cmsg_nxthdr(
struct ulinux_msg_hdr *msg,struct ulinux_cmsg_hdr *cmsg)
{
	return __ulinux_cmsg_nxthdr(msg->ctl,msg->ctl_len,cmsg);
}

#define ULINUX_MSG_OOB          1
#define ULINUX_MSG_PEEK         2
#define ULINUX_MSG_DONTROUTE    4
#define ULINUX_MSG_TRYHARD      4/*synonym for msg_dontroute for decnet*/
#define ULINUX_MSG_CTRUNC       8
#define ULINUX_MSG_PROBE        0x10/*do not send. only probe path f.e. for
                                      mtu*/
#define ULINUX_MSG_TRUNC        0x20
#define ULINUX_MSG_DONTWAIT     0x40/*nonblocking io*/
#define ULINUX_MSG_EOR          0x80/*end of record*/
#define ULINUX_MSG_WAITALL      0x100/*wait for a full request*/
#define ULINUX_MSG_FIN          0x200
#define ULINUX_MSG_SYN          0x400
#define ULINUX_MSG_CONFIRM      0x800/*confirm path validity*/
#define ULINUX_MSG_RST          0x1000
#define ULINUX_MSG_ERRQUEUE     0x2000/*fetch message from error queue*/
#define ULINUX_MSG_NOSIGNAL     0x4000/*do not generate sigpipe*/
#define ULINUX_MSG_MORE         0x8000/*sender will send more*/
#define ULINUX_MSG_WAITFORONE   0x10000/*recvmmsg(): block until 1+ packets
                                         avail*/
#define ULINUX_MSG_EOF          ULINUX_MSG_FIN
#define ULINUX_MSG_CMSG_CLOEXEC 0x40000000/*set close_on_exit for file
                                            descriptor received through
                                            scm_rights*/

/*"socket"-level control message types:*/
#define	ULINUX_SCM_RIGHTS      0x01/*rw: access rights (array of int)*/
#define ULINUX_SCM_CREDENTIALS 0x02/*rw: struct ucred*/
#define ULINUX_SCM_SECURITY    0x03/*rw: security label*/
struct ulinux_ucred{
  ulinux_u32 pid;
  ulinux_u32 uid;
  ulinux_u32 gid;
};
#endif
