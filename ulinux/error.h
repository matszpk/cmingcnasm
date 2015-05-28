#ifndef ULINUX_ERROR_H
#define ULINUX_ERROR_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/

#define ULINUX_ISERR(x) (-4095<=(x)&&(x)<=-1)
#define ULINUX_EPERM             1/*Operation not permitted*/
#define ULINUX_ENOENT            2/*No such file or directory*/
#define ULINUX_ESRCH             3/*No such process*/
#define ULINUX_EINTR             4/*Interrupted system call*/
#define ULINUX_EIO               5/*I/O error*/
#define ULINUX_ENXIO             6/*No such device or address*/
#define ULINUX_E2BIG             7/*Argument list too long*/
#define ULINUX_ENOEXEC           8/*Exec format error*/
#define ULINUX_EBADF             9/*Bad file number*/
#define ULINUX_ECHILD           10/*No child processes*/
#define ULINUX_EAGAIN           11/*Try again*/
#define ULINUX_ENOMEM           12/*Out of memory*/
#define ULINUX_EACCES           13/*Permission denied*/
#define ULINUX_EFAULT           14/*Bad address*/
#define ULINUX_ENOTBLK          15/*Block device required*/
#define ULINUX_EBUSY            16/*Device or resource busy*/
#define ULINUX_EEXIST           17/*File exists*/
#define ULINUX_EXDEV            18/*Cross-device link*/
#define ULINUX_ENODEV           19/*No such device*/
#define ULINUX_ENOTDIR          20/*Not a directory*/
#define ULINUX_EISDIR           21/*Is a directory*/
#define ULINUX_EINVAL           22/*Invalid argument*/
#define ULINUX_ENFILE           23/*File table overflow*/
#define ULINUX_EMFILE           24/*Too many open files*/
#define ULINUX_ENOTTY           25/*Not a typewriter*/
#define ULINUX_ETXTBSY          26/*Text file busy*/
#define ULINUX_EFBIG            27/*File too large*/
#define ULINUX_ENOSPC           28/*No space left on device*/
#define ULINUX_ESPIPE           29/*Illegal seek*/
#define ULINUX_EROFS            30/*Read-only file system*/
#define ULINUX_EMLINK           31/*Too many links*/
#define ULINUX_EPIPE            32/*Broken pipe*/
#define ULINUX_EDOM             33/*Math argument out of domain of func*/
#define ULINUX_ERANGE           34/*Math result not representable*/
#define ULINUX_EDEADLK          35/*Resource deadlock would occur*/
#define ULINUX_ENAMETOOLONG     36/*File name too long*/
#define ULINUX_ENOLCK           37/*No record locks available*/
#define ULINUX_ENOSYS           38/*Function not implemented*/
#define ULINUX_ENOTEMPTY        39/*Directory not empty*/
#define ULINUX_ELOOP            40/*Too many symbolic links encountered*/
#define ULINUX_EWOULDBLOCK      ULINUX_EAGAIN//Operation would block
#define ULINUX_ENOMSG           42/*No message of desired type*/
#define ULINUX_EIDRM            43/*Identifier removed*/
#define ULINUX_ECHRNG           44/*Channel number out of range*/
#define ULINUX_EL2NSYNC         45/*Level 2 not synchronized*/
#define ULINUX_EL3HLT           46/*Level 3 halted*/
#define ULINUX_EL3RST           47/*Level 3 reset*/
#define ULINUX_ELNRNG           48/*Link number out of range*/
#define ULINUX_EUNATCH          49/*Protocol driver not attached*/
#define ULINUX_ENOCSI           50/*No CSI structure available*/
#define ULINUX_EL2HLT           51/*Level 2 halted*/
#define ULINUX_EBADE            52/*Invalid exchange*/
#define ULINUX_EBADR            53/*Invalid request descriptor*/
#define ULINUX_EXFULL           54/*Exchange full*/
#define ULINUX_ENOANO           55/*No anode*/
#define ULINUX_EBADRQC          56/*Invalid request code*/
#define ULINUX_EBADSLT          57/*Invalid slot*/

#define ULINUX_EDEADLOCK        ULINUX_EDEADLK

#define ULINUX_EBFONT           59/*Bad font file format*/
#define ULINUX_ENOSTR           60/*Device not a stream*/
#define ULINUX_ENODATA          61/*No data available*/
#define ULINUX_ETIME            62/*Timer expired*/
#define ULINUX_ENOSR            63/*Out of streams resources*/
#define ULINUX_ENONET           64/*Machine is not on the network*/
#define ULINUX_ENOPKG           65/*Package not installed*/
#define ULINUX_EREMOTE          66/*Object is remote*/
#define ULINUX_ENOLINK          67/*Link has been severed*/
#define ULINUX_EADV             68/*Advertise error*/
#define ULINUX_ESRMNT           69/*Srmount error*/
#define ULINUX_ECOMM            70/*Communication error on send*/
#define ULINUX_EPROTO           71/*Protocol error*/
#define ULINUX_EMULTIHOP        72/*Multihop attempted*/
#define ULINUX_EDOTDOT          73/*RFS specific error*/
#define ULINUX_EBADMSG          74/*Not a data message*/
#define ULINUX_EOVERFLOW        75/*Value too large for defined data type*/
#define ULINUX_ENOTUNIQ         76/*Name not unique on network*/
#define ULINUX_EBADFD           77/*File descriptor in bad state*/
#define ULINUX_EREMCHG          78/*Remote address changed*/
#define ULINUX_ELIBACC          79/*Can not access a needed shared library*/
#define ULINUX_ELIBBAD          80/*Accessing a corrupted shared library*/
#define ULINUX_ELIBSCN          81/*.lib section in a.out corrupted*/
#define ULINUX_ELIBMAX          82/*Attempting to link in too many shared libraries*/
#define ULINUX_ELIBEXEC         83/*Cannot exec a shared library directly*/
#define ULINUX_EILSEQ           84/*Illegal byte sequence*/
#define ULINUX_ERESTART         85/*Interrupted system call should be restarted*/
#define ULINUX_ESTRPIPE         86/*Streams pipe error*/
#define ULINUX_EUSERS           87/*Too many users*/
#define ULINUX_ENOTSOCK         88/*Socket operation on non-socket*/
#define ULINUX_EDESTADDRREQ     89/*Destination address required*/
#define ULINUX_EMSGSIZE         90/*Message too long*/
#define ULINUX_EPROTOTYPE       91/*Protocol wrong type for socket*/
#define ULINUX_ENOPROTOOPT      92/*Protocol not available*/
#define ULINUX_EPROTONOSUPPORT  93/*Protocol not supported*/
#define ULINUX_ESOCKTNOSUPPORT  94/*Socket type not supported*/
#define ULINUX_EOPNOTSUPP       95/*Operation not supported on transport endpoint*/
#define ULINUX_EPFNOSUPPORT     96/*Protocol family not supported*/
#define ULINUX_EAFNOSUPPORT     97/*Address family not supported by protocol*/
#define ULINUX_EADDRINUSE       98/*Address already in use*/
#define ULINUX_EADDRNOTAVAIL    99/*Cannot assign requested address*/
#define ULINUX_ENETDOWN        100/*Network is down*/
#define ULINUX_ENETUNREACH     101/*Network is unreachable*/
#define ULINUX_ENETRESET i     102/*Network dropped connection because of reset*/
#define ULINUX_ECONNABORTED    103/*Software caused connection abort*/
#define ULINUX_ECONNRESET      104/*Connection reset by peer*/
#define ULINUX_ENOBUFS         105/*No buffer space available*/
#define ULINUX_EISCONN         106/*Transport endpoint is already connected*/
#define ULINUX_ENOTCONN        107/*Transport endpoint is not connected*/
#define ULINUX_ESHUTDOWN       108/*Cannot send after transport endpoint shutdown*/
#define ULINUX_ETOOMANYREFS    109/*Too many references: cannot splice*/
#define ULINUX_ETIMEDOUT       110/*Connection timed out*/
#define ULINUX_ECONNREFUSED    111/*Connection refused*/
#define ULINUX_EHOSTDOWN       112/*Host is down*/
#define ULINUX_EHOSTUNREACH    113/*No route to host*/
#define ULINUX_EALREADY        114/*Operation already in progress*/
#define ULINUX_EINPROGRESS     115/*Operation now in progress*/
#define ULINUX_ESTALE          116/*Stale NFS file handle*/
#define ULINUX_EUCLEAN         117/*Structure needs cleaning*/
#define ULINUX_ENOTNAM         118/*Not a XENIX named type file*/
#define ULINUX_ENAVAIL         119/*No XENIX semaphores available*/
#define ULINUX_EISNAM          120/*Is a named type file*/
#define ULINUX_EREMOTEIO       121/*Remote I/O error*/
#define ULINUX_EDQUOT          122/*Quota exceeded*/

#define ULINUX_ENOMEDIUM       123/*No medium found*/
#define ULINUX_EMEDIUMTYPE     124/*Wrong medium type*/
#define ULINUX_ECANCELED       125/*Operation Canceled*/
#define ULINUX_ENOKEY          126/*Required key not available*/
#define ULINUX_EKEYEXPIRED     127/*Key has expired*/
#define ULINUX_EKEYREVOKED     128/*Key has been revoked*/
#define ULINUX_EKEYREJECTED    129/*Key was rejected by service*/

/*for robust mutexes*/
#define ULINUX_EOWNERDEAD      130/*Owner died*/
#define ULINUX_ENOTRECOVERABLE 131/*State not recoverable*/

#define ULINUX_ERFKILL         132/*Operation not possible due to RF-kill*/

#endif
