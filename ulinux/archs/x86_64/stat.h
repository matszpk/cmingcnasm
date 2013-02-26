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
  k_ul nlink;
  
  k_u  mode;
  k_u  uid;
  k_u  gid;
  k_u  __pad0;
  k_ul rdev;
  k_l  sz;
  k_l  blk_sz;
  k_l  blks;//Number 512-byte blocks allocated.
  
  k_ul atime;
  k_ul atime_nsec;
  k_ul mtime;
  k_ul mtime_nsec;
  k_ul ctime;
  k_ul ctime_nsec;

  k_l  __unused[3];
};
#endif
