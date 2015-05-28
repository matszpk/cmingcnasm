#ifndef ULINUX_IOCTL_H
#define ULINUX_IOCTL_H

/*
this is a derived version of linux headers for ulinux

ioctl command encoding: 32 bits total, command in lower 16 bits,
size of the parameter structure in the lower 14 bits of the
upper 16 bits.
Encoding the size of the parameter structure in the ioctl request
is useful for catching programs compiled with old versions
and to avoid overwriting user space outside the user buffer area.
The highest 2 bits are reserved for indicating the ``access mode''.
NOTE: This limits the max parameter size to 16kB -1 !

The following is for compatibility across the various Linux
platforms.  The generic ioctl numbering scheme doesn't really enforce
a type field.  De facto, however, the top 8 bits of the lower 16
bits are indeed used as a type field, so we might just as well make
this explicit here.  Please be sure to use the decoding macros
below from now on.
*/

#define ULINUX_IOC_NRBITS   8
#define ULINUX_IOC_TYPEBITS 8

/*
Let any architecture override either of the following before
including this file.
*/

/*XXX:can be arch dependent*/
#define ULINUX_IOC_SIZEBITS 14

/*XXX:can be arch dependent*/
#define ULINUX_IOC_DIRBITS 2

#define ULINUX_IOC_NRMASK   ((1<<ULINUX_IOC_NRBITS)-1)
#define ULINUX_IOC_TYPEMASK ((1<<ULINUX_IOC_TYPEBITS)-1)
#define ULINUX_IOC_SIZEMASK	((1<<ULINUX_IOC_SIZEBITS)-1)
#define ULINUX_IOC_DIRMASK	((1<<ULINUX_IOC_DIRBITS)-1)

#define ULINUX_IOC_NRSHIFT   0
#define ULINUX_IOC_TYPESHIFT (ULINUX_IOC_NRSHIFT+ULINUX_IOC_NRBITS)
#define ULINUX_IOC_SIZESHIFT (ULINUX_IOC_TYPESHIFT+ULINUX_IOC_TYPEBITS)
#define ULINUX_IOC_DIRSHIFT  (ULINUX_IOC_SIZESHIFT+ULINUX_IOC_SIZEBITS)

/*
Direction bits, which any architecture can choose to override
before including this file.
*/

/*XXX:can be arch dependent*/
#define ULINUX_IOC_NONE 0U

/*XXX:can be arch dependent*/
#define ULINUX_IOC_WRITE 1U

/*XXX:can be arch dependent*/
#define ULINUX_IOC_READ 2U

#define ULINUX_IOC(dir,type,nr,size) \
 (((dir)<<ULINUX_IOC_DIRSHIFT) | \
 ((type)<<ULINUX_IOC_TYPESHIFT) | \
 ((nr)<<ULINUX_IOC_NRSHIFT) | \
 ((size)<<ULINUX_IOC_SIZESHIFT))

#define ULINUX_IOC_TYPECHECK(t) (sizeof(t))

/*used to create numbers*/
#define ULINUX_IO(type,nr) ULINUX_IOC(ULINUX_IOC_NONE,(type),(nr),0)
#define ULINUX_IOR(type,nr,size)\
 ULINUX_IOC(ULINUX_IOC_READ,(type),(nr),(ULINUX_IOC_TYPECHECK(size)))
#define ULINUX_IOW(type,nr,size)\
 ULINUX_IOC(ULINUX_IOC_WRITE,(type),(nr),(ULINUX_IOC_TYPECHECK(size)))
#define ULINUX_IOWR(type,nr,size)\
 ULINUX_IOC(ULINUX_IOC_READ|ULINUX_IOC_WRITE,(type),(nr),\
(ULINUX_IOC_TYPECHECK(size)))
#define ULINUX_IOR_BAD(type,nr,size) ULINUX_IOC(ULINUX_IOC_READ,(type),(nr),\
sizeof(size))
#define ULINUX_IOW_BAD(type,nr,size) ULINUX_IOC(ULINUX_IOC_WRITE,(type),(nr),\
sizeof(size))
#define ULINUX_IOWR_BAD(type,nr,size)\
 ULINUX_IOC(ULINUX_IOC_READ|ULINUX_IOC_WRITE,(type),(nr),sizeof(size))

/*used to decode ioctl numbers*/
#define ULINUX_IOC_DIR(nr) (((nr)>>ULINUX_IOC_DIRSHIFT)&ULINUX_IOC_DIRMASK)
#define ULINUX_IOC_TYPE(nr) (((nr)>>ULINUX_IOC_TYPESHIFT)&ULINUX_IOC_TYPEMASK)
#define ULINUX_IOC_NR(nr) (((nr)>>ULINUX_IOC_NRSHIFT)&ULINUX_IOC_NRMASK)
#define ULINUX_IOC_SIZE(nr) (((nr)>>ULINUX_IOC_SIZESHIFT)&ULINUX_IOC_SIZEMASK)

/*...and for the drivers/sound files...*/
#define ULINUX_IOC_IN (ULINUX_IOC_WRITE<<ULINUX_IOC_DIRSHIFT)
#define ULINUX_IOC_OUT (ULINUX_IOC_READ<<ULINUX_IOC_DIRSHIFT)
#define ULINUX_IOC_INOUT ((ULINUX_IOC_WRITE|ULINUX_IOC_READ)<<\
ULINUX_IOC_DIRSHIFT)
#define ULINUX_IOCSIZE_MASK (ULINUX_IOC_SIZEMASK<<ULINUX_IOC_SIZESHIFT)
#define ULINUX_IOCSIZE_SHIFT (ULINUX_IOC_SIZESHIFT)
#endif
