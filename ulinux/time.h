#ifndef ULINUX_TIME_H
#define ULINUX_TIME_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/
struct ulinux_timespec{
  ulinux_sl sec;/*seconds*/
  ulinux_sl nsec;/*nanoseconds*/
};

struct ulinux_timeval{
  ulinux_sl sec;/*seconds*/
  ulinux_sl usec;/*micro seconds, type can be arch dependent*/
};
#endif
