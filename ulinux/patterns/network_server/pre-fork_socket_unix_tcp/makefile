.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native 

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

pre-fork_socket_unix_tcp:pre-fork_socket_unix_tcp.c ../../../../ulinux/arch
	cpp -Wall -Wextra -I../../../.. ../../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_mem.c -o mem.o
	cpp -Wall -Wextra -I../../../.. ../../../utils/ascii/string/vsprintf.c \
		-o cpp_vsprintf.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_vsprintf.c -o vsprintf.o
	as ../../../arch/args_env.s -o args_env.o
	cpp -Wall -Wextra -I../../../.. ../../../utils/ascii/block/conv/net/net.c \
		-o cpp_net.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_net.c -o net.o
	cpp -Wall -Wextra -I../../../.. \
		../../../utils/ascii/string/conv/decimal/decimal.c \
		-o cpp_string_decimal.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c  cpp_string_decimal.c \
		-o string_decimal.o
	cpp -Wall -Wextra -I../../../.. \
		../../../utils/ascii/block/conv/decimal/decimal.c \
		-o cpp_block_decimal.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_block_decimal.c \
		-o block_decimal.o
	cpp -Wall -Wextra -I../../../.. pre-fork_socket_unix_tcp.c \
		-o cpp_pre-fork_socket_unix_tcp.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c \
		cpp_pre-fork_socket_unix_tcp.c -o pre-fork_socket_unix_tcp.o
	ld -O -nostdlib -Bstatic --strip-all block_decimal.o string_decimal.o \
		net.o args_env.o string.o mem.o vsprintf.o \
		pre-fork_socket_unix_tcp.o \
		$$(gcc -print-libgcc-file-name) \
		--output pre-fork_socket_unix_tcp

../../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../../ulinux/arch

clean:
	-rm -f ../../../../ulinux/arch
	-rm -f cpp_pre-fork_socket_unix_tcp.c \
		pre-fork_socket_unix_tcp.o \
		pre-fork_socket_unix_tcp \
		cpp_mem.c mem.o \
		cpp_vsprintf.c vsprintf.o \
		args_env.o \
		cpp_net.c net.o \
		cpp_string_decimal.c string_decimal.o \
		cpp_block_decimal.c block_decimal.o
