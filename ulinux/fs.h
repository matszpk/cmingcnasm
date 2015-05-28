#ifndef ULINUX_FS_H
#define ULINUX_FS_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#define ULINUX_MS_RDONLY      1/*mount read-only*/
#define ULINUX_MS_NOSUID      2/*ignore suid and sgid bits*/
#define ULINUX_MS_NODEV       4/*disallow access to device special files*/
#define ULINUX_MS_NOEXEC      8/*disallow program execution*/
#define ULINUX_MS_SYNCHRONOUS 16/*writes are synced at once*/
#define ULINUX_MS_REMOUNT     32/*alter flags of a mounted fs*/
#define ULINUX_MS_MANDLOCK    64/*allow mandatory locks on an fs*/
#define ULINUX_MS_DIRSYNC     128/*directory modifications are synchronous*/
#define ULINUX_MS_NOATIME     1024/*do not update access times*/
#define ULINUX_MS_NODIRATIME  2048/*do not update directory access times*/
#define ULINUX_MS_BIND        4096
#define ULINUX_MS_MOVE        8192
#define ULINUX_MS_REC         16384
#define ULINUX_MS_VERBOSE     32768/*MS_VERBOSE is deprecated*/
#define ULINUX_MS_SILENT      32768
#define ULINUX_MS_POSIXACL    (1<<16)/*vfs does not apply the umask*/
#define ULINUX_MS_UNBINDABLE  (1<<17)/*change to unbindable*/
#define ULINUX_MS_PRIVATE     (1<<18)/*change to private*/
#define ULINUX_MS_SLAVE       (1<<19)/*change to slave*/
#define ULINUX_MS_SHARED      (1<<20)/*change to shared*/
#define ULINUX_MS_RELATIME    (1<<21)/*update atime relative to mtime/ctime*/
#define ULINUX_MS_KERNMOUNT   (1<<22)/*this is a kern_mount call*/
#define ULINUX_MS_I_VERSION   (1<<23)/*update inode i_version field*/
#define ULINUX_MS_STRICTATIME (1<<24)/*always perform atime updates*/
#define ULINUX_MS_NOSEC       (1<<28)
#define ULINUX_MS_BORN        (1<<29)
#define ULINUX_MS_ACTIVE      (1<<30)
#define ULINUX_MS_NOUSER      (1<<31)

#define ULINUX_DT_UNKNOWN 0
#define ULINUX_DT_FIFO    1
#define ULINUX_DT_CHR     2
#define ULINUX_DT_DIR     4
#define ULINUX_DT_BLK     6
#define ULINUX_DT_REG     8
#define ULINUX_DT_LNK     10
#define ULINUX_DT_SOCK    12
#define ULINUX_DT_WHT     14

#define ULINUX_AT_REMOVEDIR 0x200
#endif
