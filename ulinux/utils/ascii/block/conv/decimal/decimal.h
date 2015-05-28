#ifndef ULINUX_UTILS_ASCII_BLOCK_CONV_DECIMAL_DECIMAL_H
#define ULINUX_UTILS_ASCII_BLOCK_CONV_DECIMAL_DECIMAL_H
/*******************************************************************************
this code is protected by the GNU affero GPLv3
author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
*******************************************************************************/
extern ulinux_u8 ulinux_dec2u8_blk(ulinux_u8 *dest,ulinux_u8 *start,
                                                                ulinux_u8 *end);
extern ulinux_u8 ulinux_dec2u16_blk(ulinux_u16 *dest,ulinux_u8 *start,
                                                                ulinux_u8 *end);
extern ulinux_u8 ulinux_dec2u32_blk(ulinux_u32 *dest,ulinux_u8 *start,
                                                                ulinux_u8 *end);
extern ulinux_u8 ulinux_dec2u64_blk(ulinux_u64 *dest,ulinux_u8 *start,
                                                                ulinux_u8 *end);
#endif
