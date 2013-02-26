#ifndef ULINUX_SOCKET_MSG_H
#define ULINUX_SOCKET_MSG_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_SOL_SOCKET 1

struct k_io_vec{//will probably go somewhere else
  void *base;//BSD uses caddr_t (1003.1g requires void *)
  k_sz len;  //Must be size_t (1003.1g)
};

struct k_msg_hdr{
  void *name;//socket name
  k_i name_len;//length of name
  struct k_io_vec *iov;//data blocks
  k_sz iov_len;//number of blocks
  void *ctl;//per protocol magic (eg BSD file descriptor passing)
  k_sz ctl_len;//length of cmsg list
  k_u flgs;
}; 

//the c prefix stands for access *c*ontrol message (other name ancillary data)
struct k_cmsg_hdr{
  k_sz len;//data byte count, including hdr
  k_i lvl;//originating protocol
  k_i type;//protocol-specific type
};

#define __K_CMSG_NXTHDR(ctl,len,cmsg) __k_cmsg_nxthdr((ctl),(len),(cmsg))
#define K_CMSG_NXTHDR(mhdr,cmsg) k_cmsg_nxthdr((mhdr),(cmsg))

#define K_CMSG_ALIGN(len) (((len)+sizeof(k_l)-1)&~(sizeof(k_l)-1))

#define K_CMSG_DATA(cmsg) ((void*)((k_u8*)(cmsg)\
                                     +K_CMSG_ALIGN( sizeof(struct k_cmsg_hdr))))
#define K_CMSG_SPACE(len) (K_CMSG_ALIGN(sizeof(struct k_cmsg_hdr))\
                                                             +K_CMSG_ALIGN(len))
#define K_CMSG_LEN(len) (K_CMSG_ALIGN(sizeof(struct k_cmsg_hdr))+(len))

#define __K_CMSG_FIRSTHDR(ctl,len) ((len)>=sizeof(struct k_cmsg_hdr) ? \
                                   (struct k_cmsg_hdr *)(ctl) : \
                                   (struct k_cmsg_hdr *)0)
#define K_CMSG_FIRSTHDR(msg) __K_CMSG_FIRSTHDR((msg)->ctl,\
                                               (msg)->ctl_len)
#define K_CMSG_OK(mhdr, cmsg) ((cmsg)->cmsg_len>=sizeof(struct cmsg_hdr)&&\
                               (cmsg)->cmsg_len<=(k_ul)\
                              ((mhdr)->ctl_len-\
                               ((k_u8*)(cmsg)-(k_u8*)(mhdr)->ctl)))

static inline struct k_cmsg_hdr * __k_cmsg_nxthdr(void *ctl,k_sz size,
                                                 struct k_cmsg_hdr *cmsg)
{
  struct k_cmsg_hdr *ptr;

  ptr=(struct k_cmsg_hdr*)(((k_u8*)cmsg)+K_CMSG_ALIGN(cmsg->len));
  if((k_ul)((k_u8*)(ptr+1)-(k_u8*)ctl)>size) return (struct k_cmsg_hdr *)0;
  return ptr;
}

static inline struct k_cmsg_hdr *k_cmsg_nxthdr(struct k_msg_hdr *msg,
                                               struct k_cmsg_hdr *cmsg)
{
	return __k_cmsg_nxthdr(msg->ctl,msg->ctl_len,cmsg);
}

#define K_MSG_OOB          1
#define K_MSG_PEEK         2
#define K_MSG_DONTROUTE    4
#define K_MSG_TRYHARD      4//synonym for msg_dontroute for decnet
#define K_MSG_CTRUNC       8
#define K_MSG_PROBE        0x10//do not send. only probe path f.e. for mtu
#define K_MSG_TRUNC        0x20
#define K_MSG_DONTWAIT     0x40//nonblocking io
#define K_MSG_EOR          0x80//end of record
#define K_MSG_WAITALL      0x100//wait for a full request
#define K_MSG_FIN          0x200
#define K_MSG_SYN          0x400
#define K_MSG_CONFIRM      0x800//confirm path validity
#define K_MSG_RST          0x1000
#define K_MSG_ERRQUEUE     0x2000//fetch message from error queue
#define K_MSG_NOSIGNAL     0x4000//do not generate sigpipe
#define K_MSG_MORE         0x8000//sender will send more
#define K_MSG_WAITFORONE   0x10000//recvmmsg(): block until 1+ packets avail
#define K_MSG_EOF          K_MSG_FIN
#define K_MSG_CMSG_CLOEXEC 0x40000000//set close_on_exit for file descriptor
                                     //received through scm_rights

//"socket"-level control message types:
#define	K_SCM_RIGHTS      0x01//rw: access rights (array of int)
#define K_SCM_CREDENTIALS 0x02//rw: struct ucred
#define K_SCM_SECURITY    0x03//rw: security label
struct ucred{
  k_u32 pid;
  k_u32 uid;
  k_u32 gid;
};
#endif
