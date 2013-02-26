//**********************************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//**********************************************************************************************
#ifndef ULINUX_SIGNAL_HANDLER_H
#define ULINUX_SIGNAL_HANDLER_H
struct k_sigaction {
	void *k_sa_handler;
	k_ul sa_flags;
	void (*sa_restorer)(void);
	k_ul sa_mask;
};
#include <ulinux/arch/signal/handler.h>
#endif
