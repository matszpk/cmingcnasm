#ifndef ULINUX_ARCH_STAT_H
#define ULINUX_ARCH_STAT_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
struct ulinux_stat{
  ulinux_ul dev;
  ulinux_ul ino;
  ulinux_ul nlink;

  ulinux_ui mode;
  ulinux_ui uid;
  ulinux_ui gid;
  ulinux_ui __pad0;
  ulinux_ul rdev;
  ulinux_sl sz;
  ulinux_sl blk_sz;
  ulinux_sl blks;/*number 512-byte blocks allocated*/

  ulinux_ul atime;
  ulinux_ul atime_nsec;
  ulinux_ul mtime;
  ulinux_ul mtime_nsec;
  ulinux_ul ctime;
  ulinux_ul ctime_nsec;

  ulinux_sl __unused[3];
};
#endif
