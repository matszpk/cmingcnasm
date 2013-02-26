#ifndef ULINUX_ARCH_MMAP_H
#define ULINUX_ARCH_MMAP_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
#define K_PAGE_SZ 4096

#define K_MAP_GROWSDOWN  0x0100//stack-like segment
#define K_MAP_DENYWRITE  0x0800//ETXTBSY
#define K_MAP_EXECUTABLE 0x1000//mark it as an executable
#define K_MAP_LOCKED     0x2000//pages are locked
#define K_MAP_NORESERVE  0x4000//don't check for reservations
#define K_MAP_POPULATE   0x8000//populate (prefault) pagetables
#define K_MAP_NONBLOCK   0x10000//do not block on IO
#define K_MAP_STACK      0x20000//give out an address that is best suited for
                                //process/thread stacks
#define K_MAP_HUGETLB    0x40000//create a huge page mapping

#define K_MCL_CURRENT 1//lock all current mappings
#define K_MCL_FUTURE  2//lock all future mappings
#endif
