#ifndef ULINUX_IOCTL_H
#define ULINUX_IOCTL_H

//this is a derived version of linux headers for ulinux

//ioctl command encoding: 32 bits total, command in lower 16 bits,
//size of the parameter structure in the lower 14 bits of the
//upper 16 bits.
//Encoding the size of the parameter structure in the ioctl request
//is useful for catching programs compiled with old versions
//and to avoid overwriting user space outside the user buffer area.
//The highest 2 bits are reserved for indicating the ``access mode''.
//NOTE: This limits the max parameter size to 16kB -1 !

//The following is for compatibility across the various Linux
//platforms.  The generic ioctl numbering scheme doesn't really enforce
//a type field.  De facto, however, the top 8 bits of the lower 16
//bits are indeed used as a type field, so we might just as well make
//this explicit here.  Please be sure to use the decoding macros
//below from now on.

#define K_IOC_NRBITS   8
#define K_IOC_TYPEBITS 8

//Let any architecture override either of the following before
//including this file.

//XXX:can be arch dependent
#define K_IOC_SIZEBITS 14

//XXX:can be arch dependent
#define K_IOC_DIRBITS 2

#define K_IOC_NRMASK   ((1<<K_IOC_NRBITS)-1)
#define K_IOC_TYPEMASK ((1<<K_IOC_TYPEBITS)-1)
#define K_IOC_SIZEMASK	((1<<K_IOC_SIZEBITS)-1)
#define K_IOC_DIRMASK	((1<<K_IOC_DIRBITS)-1)

#define K_IOC_NRSHIFT   0
#define K_IOC_TYPESHIFT (K_IOC_NRSHIFT+K_IOC_NRBITS)
#define K_IOC_SIZESHIFT (K_IOC_TYPESHIFT+K_IOC_TYPEBITS)
#define K_IOC_DIRSHIFT  (K_IOC_SIZESHIFT+K_IOC_SIZEBITS)

//Direction bits, which any architecture can choose to override
//before including this file.

//XXX:can be arch dependent
#define K_IOC_NONE 0U

//XXX:can be arch dependent
#define K_IOC_WRITE 1U

//XXX:can be arch dependent
#define K_IOC_READ 2U

#define K_IOC(dir,type,nr,size) \
 (((dir)<<K_IOC_DIRSHIFT) | \
 ((type)<<K_IOC_TYPESHIFT) | \
 ((nr)<<K_IOC_NRSHIFT) | \
 ((size)<<K_IOC_SIZESHIFT))

#define K_IOC_TYPECHECK(t) (sizeof(t))

//used to create numbers
#define K_IO(type,nr) K_IOC(K_IOC_NONE,(type),(nr),0)
#define K_IOR(type,nr,size)\
 K_IOC(K_IOC_READ,(type),(nr),(K_IOC_TYPECHECK(size)))
#define K_IOW(type,nr,size)\
 K_IOC(K_IOC_WRITE,(type),(nr),(K_IOC_TYPECHECK(size)))
#define K_IOWR(type,nr,size)\
 K_IOC(K_IOC_READ|K_IOC_WRITE,(type),(nr),(K_IOC_TYPECHECK(size)))
#define K_IOR_BAD(type,nr,size) K_IOC(K_IOC_READ,(type),(nr),sizeof(size))
#define K_IOW_BAD(type,nr,size) K_IOC(K_IOC_WRITE,(type),(nr),sizeof(size))
#define K_IOWR_BAD(type,nr,size)\
 K_IOC(K_IOC_READ|K_IOC_WRITE,(type),(nr),sizeof(size))

//used to decode ioctl numbers.
#define K_IOC_DIR(nr) (((nr)>>K_IOC_DIRSHIFT)&K_IOC_DIRMASK)
#define K_IOC_TYPE(nr) (((nr)>>K_IOC_TYPESHIFT)&K_IOC_TYPEMASK)
#define K_IOC_NR(nr) (((nr)>>K_IOC_NRSHIFT)&K_IOC_NRMASK)
#define K_IOC_SIZE(nr) (((nr)>>K_IOC_SIZESHIFT)&K_IOC_SIZEMASK)

//...and for the drivers/sound files...
#define K_IOC_IN (K_IOC_WRITE<<K_IOC_DIRSHIFT)
#define K_IOC_OUT (K_IOC_READ<<K_IOC_DIRSHIFT)
#define K_IOC_INOUT ((K_IOC_WRITE|K_IOC_READ)<<K_IOC_DIRSHIFT)
#define K_IOCSIZE_MASK (K_IOC_SIZEMASK<<K_IOC_SIZESHIFT)
#define K_IOCSIZE_SHIFT (K_IOC_SIZESHIFT)
#endif
