/*******************************************************************************
this code is protected by the GNU affero GPLv3 with a lesser exception
for video games
author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
*******************************************************************************/

#define u8 ulinux_u8
#define s8 ulinux_s8
#define s16 ulinux_s16
#define u16 ulinux_u16
#define s32 ulinux_s32
#define u32 ulinux_u32
#define u64 ulinux_u64
#define si ulinux_si
#define sl ulinux_sl
#define ul ulinux_ul
#define mmap(a,b,c,d) ulinux_sysc(mmap,6,0,a,b,c,d,0)
#define mremap(a,b,c,d) ulinux_sysc(mremap,5,a,b,c,d,0)
#define munmap(a,b) ulinux_sysc(munmap,2,a,b)
#define PROT_READ ULINUX_PROT_READ
#define PROT_WRITE ULINUX_PROT_WRITE
#define MAP_PRIVATE ULINUX_MAP_PRIVATE
#define MAP_ANONYMOUS ULINUX_MAP_ANONYMOUS
#define ISERR ULINUX_ISERR
#define MREMAP_MAYMOVE ULINUX_MREMAP_MAYMOVE
#define strncpy(a,b,c) ulinux_strncpy(a,(u8*)b,c)
#define strncmp(a,b,c) ulinux_strncmp((u8*)a,(u8*)b,c)
#define is_digit ulinux_is_digit
#define dec2u8 ulinux_dec2u8_blk
#define dec2u16 ulinux_dec2u16_blk
#define dec2u64 ulinux_dec2u64_blk
#define bin2u16 ulinux_bin2u16_blk
#define hex2u16 ulinux_hex2u16_blk
#define cpu2le64 ulinux_cpu2le64
#define cpu2le32 ulinux_cpu2le32
#define le322cpu ulinux_le322cpu
#define le642cpu ulinux_le642cpu
#define loop while(1)
#define vsnprintf ulinux_vsnprintf
#define snprintf(a,b,c,...) ulinux_snprintf(a,b,(u8*)c,##__VA_ARGS__)
#define exit(a) ulinux_sysc(exit_group,1,a)
#define open(a,b,c) ulinux_sysc(open,3,a,b,c)
#define O_RDONLY ULINUX_O_RDONLY
#define O_CREAT ULINUX_O_CREAT
#define O_TRUNC ULINUX_O_TRUNC
#define O_WRONLY ULINUX_O_WRONLY
#define S_IRUSR ULINUX_S_IRUSR
#define S_IWUSR ULINUX_S_IWUSR
#define S_IRGRP ULINUX_S_IRGRP
#define S_IROTH ULINUX_S_IROTH
#define EINTR ULINUX_EINTR
#define EAGAIN ULINUX_EAGAIN
#define stat ulinux_stat
#define fstat(a,b) ulinux_sysc(fstat,2,a,b)
#define write(a,b,c) ulinux_sysc(write,3,a,b,c)
#define memcmp ulinux_memcmp
