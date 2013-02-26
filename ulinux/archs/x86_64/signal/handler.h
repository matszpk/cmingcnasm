//**********************************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//**********************************************************************************************
#ifndef ULINUX_ARCH_SIGNAL_HANDLER_H
#define ULINUX_ARCH_SIGNAL_HANDLER_H
#define K_SA_RESTORER 0x04000000
extern k_l arch_sigaction(k_l num,struct k_sigaction *kact,struct k_sigaction *okact);
#endif
