/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#ifndef ULINUX_SIGNAL_HANDLER_H
#define ULINUX_SIGNAL_HANDLER_H
struct ulinux_sigaction {
	void *ulinux_sa_handler;
	ulinux_ul sa_flags;
	void (*sa_restorer)(void);
	ulinux_ul sa_mask;
};
#include <ulinux/arch/signal/handler.h>
#endif
