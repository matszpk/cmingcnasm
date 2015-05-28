/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#ifndef ULINUX_ARCH_SIGNAL_HANDLER_H
#define ULINUX_ARCH_SIGNAL_HANDLER_H
#define ULINUX_SA_RESTORER 0x04000000
extern ulinux_sl arch_sigaction(ulinux_sl num,struct ulinux_sigaction *kact,
                                                struct ulinux_sigaction *okact);
#endif
