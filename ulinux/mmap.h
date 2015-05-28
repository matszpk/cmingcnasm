#ifndef ULINUX_MMAP_H
#define ULINUX_MMAP_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
#include <ulinux/arch/mmap.h>

#define ULINUX_PROT_READ      0x1/*page can be read*/
#define ULINUX_PROT_WRITE     0x2/*page can be written*/
#define ULINUX_PROT_EXEC      0x4/*page can be executed*/
#define ULINUX_PROT_SEM       0x8/*page may be used for atomic ops*/
#define ULINUX_PROT_NONE      0x0/*page can not be accessed*/
#define ULINUX_PROT_GROWSDOWN 0x01000000/*mprotect flag: extend change to start
                                          of growsdown vma*/
#define ULINUX_PROT_GROWSUP   0x02000000/*mprotect flag: extend change to end of
                                          growsup vma*/

#define ULINUX_MAP_SHARED    0x01/*Share changes*/
#define ULINUX_MAP_PRIVATE   0x02/*Changes are private*/
#define ULINUX_MAP_TYPE      0x0f/*Mask for type of mapping*/
#define ULINUX_MAP_FIXED     0x10/*Interpret addr exactly*/
#define ULINUX_MAP_ANONYMOUS 0x20/*don't use a file*/

#define ULINUX_MREMAP_MAYMOVE 0x1
#define ULINUX_MREMAP_FIXED   0x2
#endif
