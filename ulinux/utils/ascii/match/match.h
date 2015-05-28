#ifndef	ULINUX_UTILS_ASCII_MATCH_H
#define ULINUX_UTILS_ASCII_MATCH_H
/*******************************************************************************
This is a derived work based on GNU glibc implementation on 20130409.
Switch to GNU Lesser GPLv3 protection.
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/

/*flags*/
#define ULINUX_MATCH_CASEFOLD (1<<0)/*compare without regard to case*/
#define ULINUX_MATCH_EXTMATCH (1<<1)/*use ksh-like extended matching*/

/*return values*/
#define ULINUX_MATCH_ERR_NOMEM         -3
#define ULINUX_MATCH_ERR               -2
#define ULINUX_MATCH_MATCH              0
#define ULINUX_MATCH_NOMATCH            1

ulinux_s8 ulinux_match(ulinux_u8 *p,ulinux_u8 *str,ulinux_u8 flgs);
#endif
