#ifndef ULINUX_DIRENT_H
#define ULINUX_DIRENT_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct k_dirent64{
  k_u64 ino;
  k_s64 off;
  k_us rec_len;
  k_u8 type;
  k_u8 name[0];
};
#endif
