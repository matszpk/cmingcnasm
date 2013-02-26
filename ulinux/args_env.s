.include "ulinux/arch/args_env.s"
#To avoid an exe stack by default, must add that line.
#ELF GNU_STACK segment should go away soon for non executable stack.
	.section	.note.GNU-stack,"",@progbits
