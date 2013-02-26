#ifndef ULINUX_ERROR_H
#define ULINUX_ERROR_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************

#define K_ISERR(x) (-4095<=(x)&&(x)<=-1)
#define K_EPERM             1//Operation not permitted
#define K_ENOENT            2//No such file or directory
#define K_ESRCH             3//No such process
#define K_EINTR             4//Interrupted system call
#define K_EIO               5//I/O error
#define K_ENXIO             6//No such device or address
#define K_E2BIG             7//Argument list too long
#define K_ENOEXEC           8//Exec format error
#define K_EBADF             9//Bad file number
#define K_ECHILD           10//No child processes
#define K_EAGAIN           11//Try again
#define K_ENOMEM           12//Out of memory
#define K_EACCES           13//Permission denied
#define K_EFAULT           14//Bad address
#define K_ENOTBLK          15//Block device required
#define K_EBUSY            16//Device or resource busy
#define K_EEXIST           17//File exists
#define K_EXDEV            18//Cross-device link
#define K_ENODEV           19//No such device
#define K_ENOTDIR          20//Not a directory
#define K_EISDIR           21//Is a directory
#define K_EINVAL           22//Invalid argument
#define K_ENFILE           23//File table overflow
#define K_EMFILE           24//Too many open files
#define K_ENOTTY           25//Not a typewriter
#define K_ETXTBSY          26//Text file busy
#define K_EFBIG            27//File too large
#define K_ENOSPC           28//No space left on device
#define K_ESPIPE           29//Illegal seek
#define K_EROFS            30//Read-only file system
#define K_EMLINK           31//Too many links
#define K_EPIPE            32//Broken pipe
#define K_EDOM             33//Math argument out of domain of func
#define K_ERANGE           34//Math result not representable
#define K_EDEADLK          35//Resource deadlock would occur
#define K_ENAMETOOLONG     36//File name too long
#define K_ENOLCK           37//No record locks available
#define K_ENOSYS           38//Function not implemented
#define K_ENOTEMPTY        39//Directory not empty
#define K_ELOOP            40//Too many symbolic links encountered
#define K_EWOULDBLOCK      K_EAGAIN//Operation would block
#define K_ENOMSG           42//No message of desired type
#define K_EIDRM            43//Identifier removed
#define K_ECHRNG           44//Channel number out of range
#define K_EL2NSYNC         45//Level 2 not synchronized
#define K_EL3HLT           46//Level 3 halted
#define K_EL3RST           47//Level 3 reset
#define K_ELNRNG           48//Link number out of range
#define K_EUNATCH          49//Protocol driver not attached
#define K_ENOCSI           50//No CSI structure available
#define K_EL2HLT           51//Level 2 halted
#define K_EBADE            52//Invalid exchange
#define K_EBADR            53//Invalid request descriptor
#define K_EXFULL           54//Exchange full
#define K_ENOANO           55//No anode
#define K_EBADRQC          56//Invalid request code
#define K_EBADSLT          57//Invalid slot

#define K_EDEADLOCK        K_EDEADLK

#define K_EBFONT           59//Bad font file format
#define K_ENOSTR           60//Device not a stream
#define K_ENODATA          61//No data available
#define K_ETIME            62//Timer expired
#define K_ENOSR            63//Out of streams resources
#define K_ENONET           64//Machine is not on the network
#define K_ENOPKG           65//Package not installed
#define K_EREMOTE          66//Object is remote
#define K_ENOLINK          67//Link has been severed
#define K_EADV             68//Advertise error
#define K_ESRMNT           69//Srmount error
#define K_ECOMM            70//Communication error on send
#define K_EPROTO           71//Protocol error
#define K_EMULTIHOP        72//Multihop attempted
#define K_EDOTDOT          73//RFS specific error
#define K_EBADMSG          74//Not a data message
#define K_EOVERFLOW        75//Value too large for defined data type
#define K_ENOTUNIQ         76//Name not unique on network
#define K_EBADFD           77//File descriptor in bad state
#define K_EREMCHG          78//Remote address changed
#define K_ELIBACC          79//Can not access a needed shared library
#define K_ELIBBAD          80//Accessing a corrupted shared library
#define K_ELIBSCN          81//.lib section in a.out corrupted
#define K_ELIBMAX          82//Attempting to link in too many shared libraries
#define K_ELIBEXEC         83//Cannot exec a shared library directly
#define K_EILSEQ           84//Illegal byte sequence
#define K_ERESTART         85//Interrupted system call should be restarted
#define K_ESTRPIPE         86//Streams pipe error
#define K_EUSERS           87//Too many users
#define K_ENOTSOCK         88//Socket operation on non-socket
#define K_EDESTADDRREQ     89//Destination address required
#define K_EMSGSIZE         90//Message too long
#define K_EPROTOTYPE       91//Protocol wrong type for socket
#define K_ENOPROTOOPT      92//Protocol not available
#define K_EPROTONOSUPPORT  93//Protocol not supported
#define K_ESOCKTNOSUPPORT  94//Socket type not supported
#define K_EOPNOTSUPP       95//Operation not supported on transport endpoint
#define K_EPFNOSUPPORT     96//Protocol family not supported
#define K_EAFNOSUPPORT     97//Address family not supported by protocol
#define K_EADDRINUSE       98//Address already in use
#define K_EADDRNOTAVAIL    99//Cannot assign requested address
#define K_ENETDOWN        100//Network is down
#define K_ENETUNREACH     101//Network is unreachable
#define K_ENETRESET i     102//Network dropped connection because of reset
#define K_ECONNABORTED    103//Software caused connection abort
#define K_ECONNRESET      104//Connection reset by peer
#define K_ENOBUFS         105//No buffer space available
#define K_EISCONN         106//Transport endpoint is already connected
#define K_ENOTCONN        107//Transport endpoint is not connected
#define K_ESHUTDOWN       108//Cannot send after transport endpoint shutdown
#define K_ETOOMANYREFS    109//Too many references: cannot splice
#define K_ETIMEDOUT       110//Connection timed out
#define K_ECONNREFUSED    111//Connection refused
#define K_EHOSTDOWN       112//Host is down
#define K_EHOSTUNREACH    113//No route to host
#define K_EALREADY        114//Operation already in progress
#define K_EINPROGRESS     115//Operation now in progress
#define K_ESTALE          116//Stale NFS file handle
#define K_EUCLEAN         117//Structure needs cleaning
#define K_ENOTNAM         118//Not a XENIX named type file
#define K_ENAVAIL         119//No XENIX semaphores available
#define K_EISNAM          120//Is a named type file
#define K_EREMOTEIO       121//Remote I/O error
#define K_EDQUOT          122//Quota exceeded

#define K_ENOMEDIUM       123//No medium found
#define K_EMEDIUMTYPE     124//Wrong medium type
#define K_ECANCELED       125//Operation Canceled
#define K_ENOKEY          126//Required key not available
#define K_EKEYEXPIRED     127//Key has expired
#define K_EKEYREVOKED     128//Key has been revoked
#define K_EKEYREJECTED    129//Key was rejected by service

//for robust mutexes
#define K_EOWNERDEAD      130//Owner died
#define K_ENOTRECOVERABLE 131//State not recoverable

#define K_ERFKILL         132//Operation not possible due to RF-kill

#endif
