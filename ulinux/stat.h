#ifndef ULINUX_STAT_H
#define ULINUX_STAT_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#include <ulinux/arch/stat.h>

#define K_S_IFSOCK 0140000
#define K_S_IFLNK  0120000
#define K_S_IFREG  0100000
#define K_S_IFBLK  0060000
#define K_S_IFDIR  0040000
#define K_S_IFCHR  0020000
#define K_S_IFIFO  0010000
#define K_S_ISUID  0004000
#define K_S_ISGID  0002000
#define K_S_ISVTX  0001000

#define K_S_IRWXU 00700
#define K_S_IRUSR 00400
#define K_S_IWUSR 00200
#define K_S_IXUSR 00100

#define K_S_IRWXG 00070
#define K_S_IRGRP 00040
#define K_S_IWGRP 00020
#define K_S_IXGRP 00010

#define K_S_IRWXO 00007
#define K_S_IROTH 00004
#define K_S_IWOTH 00002
#define K_S_IXOTH 00001

#define K_S_IRWXUGO (K_S_IRWXU|K_S_IRWXG|K_S_IRWXO)
#define K_S_IALLUGO (K_S_ISUID|K_S_ISGID|K_S_ISVTX|K_S_IRWXUGO)
#define K_S_IRUGO   (K_S_IRUSR|K_S_IRGRP|K_S_IROTH)
#define K_S_IWUGO   (K_S_IWUSR|K_S_IWGRP|K_S_IWOTH)
#define K_S_IXUGO   (K_S_IXUSR|K_S_IXGRP|K_S_IXOTH)
#endif
