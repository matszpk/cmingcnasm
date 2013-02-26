#ifndef ULINUX_FS_H
#define ULINUX_FS_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_MS_RDONLY      1//mount read-only
#define K_MS_NOSUID      2//ignore suid and sgid bits
#define K_MS_NODEV       4//disallow access to device special files
#define K_MS_NOEXEC      8//disallow program execution
#define K_MS_SYNCHRONOUS 16//writes are synced at once
#define K_MS_REMOUNT     32//alter flags of a mounted fs
#define K_MS_MANDLOCK    64//allow mandatory locks on an fs
#define K_MS_DIRSYNC     128//directory modifications are synchronous
#define K_MS_NOATIME     1024//do not update access times
#define K_MS_NODIRATIME  2048//do not update directory access times
#define K_MS_BIND        4096
#define K_MS_MOVE        8192
#define K_MS_REC         16384
#define K_MS_VERBOSE     32768//MS_VERBOSE is deprecated
#define K_MS_SILENT      32768
#define K_MS_POSIXACL    (1<<16)//vfs does not apply the umask
#define K_MS_UNBINDABLE  (1<<17)//change to unbindable
#define K_MS_PRIVATE     (1<<18)//change to private
#define K_MS_SLAVE       (1<<19)//change to slave
#define K_MS_SHARED      (1<<20)//change to shared
#define K_MS_RELATIME    (1<<21)//update atime relative to mtime/ctime
#define K_MS_KERNMOUNT   (1<<22)//this is a kern_mount call
#define K_MS_I_VERSION   (1<<23)//update inode i_version field
#define K_MS_STRICTATIME (1<<24)//always perform atime updates
#define K_MS_NOSEC       (1<<28)
#define K_MS_BORN        (1<<29)
#define K_MS_ACTIVE      (1<<30)
#define K_MS_NOUSER      (1<<31)

#define K_DT_UNKNOWN 0
#define K_DT_FIFO    1
#define K_DT_CHR     2
#define K_DT_DIR     4
#define K_DT_BLK     6
#define K_DT_REG     8
#define K_DT_LNK     10
#define K_DT_SOCK    12
#define K_DT_WHT     14

#define K_AT_REMOVEDIR 0x200
#endif
