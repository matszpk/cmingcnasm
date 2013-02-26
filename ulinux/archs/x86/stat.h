#ifndef ULINUX_ARCH_STAT_H
#define ULINUX_ARCH_STAT_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct k_stat{
  k_ul dev;
  k_ul ino;
  k_us mode;
  k_us nlink;
  k_us uid;
  k_us gid;
  k_ul rdev;
  k_ul sz;
  k_ul blk_sz;
  k_ul blks;
  
  k_ul atime;
  k_ul atime_nsec;
  k_ul mtime;
  k_ul mtime_nsec;
  k_ul ctime;
  k_ul ctime_nsec;
  
  k_ul __unused4;
  k_ul __unused5;
};
#endif
