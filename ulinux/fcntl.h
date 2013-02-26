#ifndef ULINUX_FCNTL_H
#define ULINUX_FCNTL_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
//some commands may be already defined for some archs
#define F_DUPFD     0//dup
#define F_GETFD     1//get close_on_exec
#define F_SETFD     2//set/clear close_on_exec
#define F_GETFL     3//get file->f_flags
#define F_SETFL     4//set file->f_flags
#ifndef F_GETLK
#define F_GETLK     5
#define F_SETLK     6
#define F_SETLKW    7
#endif
#ifndef F_SETOWN
#define F_SETOWN    8//for sockets
#define F_GETOWN    9//for sockets
#endif
#ifndef F_SETSIG
#define F_SETSIG    10//for sockets
#define F_GETSIG    11//for sockets
#endif
#ifndef F_SETOWN_EX
#define F_SETOWN_EX 15
#define F_GETOWN_EX 16
#endif
#endif
