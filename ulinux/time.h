#ifndef ULINUX_TIME_H
#define ULINUX_TIME_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
struct k_timespec{
  k_l sec;//seconds
  k_l nsec;//nanoseconds
};

struct k_timeval{
  k_l sec;//seconds
  k_l usec;//micro seconds, type can be arch dependent
};
#endif
