#ifndef ULINUX_STAT_H
#define ULINUX_STAT_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#include <ulinux/arch/stat.h>

#define ULINUX_S_IFSOCK 0140000
#define ULINUX_S_IFLNK  0120000
#define ULINUX_S_IFREG  0100000
#define ULINUX_S_IFBLK  0060000
#define ULINUX_S_IFDIR  0040000
#define ULINUX_S_IFCHR  0020000
#define ULINUX_S_IFIFO  0010000
#define ULINUX_S_ISUID  0004000
#define ULINUX_S_ISGID  0002000
#define ULINUX_S_ISVTX  0001000

#define ULINUX_S_IRWXU 00700
#define ULINUX_S_IRUSR 00400
#define ULINUX_S_IWUSR 00200
#define ULINUX_S_IXUSR 00100

#define ULINUX_S_IRWXG 00070
#define ULINUX_S_IRGRP 00040
#define ULINUX_S_IWGRP 00020
#define ULINUX_S_IXGRP 00010

#define ULINUX_S_IRWXO 00007
#define ULINUX_S_IROTH 00004
#define ULINUX_S_IWOTH 00002
#define ULINUX_S_IXOTH 00001

#define ULINUX_S_IRWXUGO (ULINUX_S_IRWXU|ULINUX_S_IRWXG|ULINUX_S_IRWXO)
#define ULINUX_S_IALLUGO (ULINUX_S_ISUID|ULINUX_S_ISGID|ULINUX_S_ISVTX\
|ULINUX_S_IRWXUGO)
#define ULINUX_S_IRUGO   (ULINUX_S_IRUSR|ULINUX_S_IRGRP|ULINUX_S_IROTH)
#define ULINUX_S_IWUGO   (ULINUX_S_IWUSR|ULINUX_S_IWGRP|ULINUX_S_IWOTH)
#define ULINUX_S_IXUGO   (ULINUX_S_IXUSR|ULINUX_S_IXGRP|ULINUX_S_IXOTH)
#endif
