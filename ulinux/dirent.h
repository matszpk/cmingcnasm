#ifndef ULINUX_DIRENT_H
#define ULINUX_DIRENT_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
struct ulinux_dirent64{
  ulinux_u64 ino;
  ulinux_s64 off;
  ulinux_us rec_len;
  ulinux_u8 type;
  ulinux_u8 name[0];
};
#endif
