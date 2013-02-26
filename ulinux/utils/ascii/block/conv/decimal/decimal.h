#ifndef ULINUX_UTILS_ASCII_BLOCK_CONV_DECIMAL_DECIMAL_H
#define ULINUX_UTILS_ASCII_BLOCK_CONV_DECIMAL_DECIMAL_H
//******************************************************************************
//*this code is protected by the GNU affero GPLv3
//*author:Sylvain BERTRAND (sylvain.bertrand AT gmail dot com)
//*                        <digital.ragnarok AT gmail dot com>
//******************************************************************************
extern k_ut u_a_strict_dec2u8_blk(k_u8 *dest,k_u8 *start,k_u8 *end);
extern k_ut u_a_strict_dec2u16_blk(k_u16 *dest,k_u8 *start,k_u8 *end);
extern k_ut u_a_strict_dec2u32_blk(k_u32 *dest,k_u8 *start,k_u8 *end);
extern k_ut u_a_strict_dec2u64_blk(k_u64 *dest,k_u8 *start,k_u8 *end);
#endif
