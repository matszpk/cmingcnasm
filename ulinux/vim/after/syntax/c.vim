" Vim syntax file
" Language:		C
" Maintainer:	Sylvain BERTRAND <sylvain.bertrand@gmail.com>
" Last Change:	2015 May 27

"compiler types
syn keyword	cType		ulinux_sc ulinux_ss ulinux_si ulinux_sl
syn keyword	cType		ulinux_sll
syn keyword	cType		ulinux_uc ulinux_us ulinux_ui ulinux_ul
syn keyword	cType		ulinux_ull
syn keyword	cType		ulinux_f

"arch types
syn keyword	cType		ulinux_u8 ulinux_u16 ulinux_u32 ulinux_u64
syn keyword	cType		ulinux_s8 ulinux_s16 ulinux_s32 ulinux_s64
syn keyword	cType		ulinux_f32

syn keyword	cType		ulinux_sz ulinux_ptrdiff

"ulinux namespaced types
syn keyword	cType		uc us ui ul
syn keyword	cType		ull
syn keyword	cType		sc ss si sl
syn keyword	cType		sll
syn keyword	cType		u8 u16 u32 u64
syn keyword	cType		s8 s16 s32 s64
syn keyword	cType		f32

"ulinux flow control
syn keyword	cRepeat		loop
syn keyword	cRepeat		ulinux_loop
