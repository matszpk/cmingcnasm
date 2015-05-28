.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native 

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

args_env:args_env.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. ../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_mem.c -o mem.o
	cpp -Wall -Wextra -I../../.. ../../utils/ascii/string/vsprintf.c \
		-o cpp_vsprintf.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_vsprintf.c -o vsprintf.o
	as -I../../.. ../../args_env.s -o arch_args_env.o
	cpp -Wall -Wextra -I../../.. args_env.c -o cpp_args_env.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_args_env.c -o args_env.o
	ld -O -nostdlib -Bstatic --strip-all args_env.o arch_args_env.o \
		string.o mem.o vsprintf.o \
		$$(gcc -print-libgcc-file-name) --output args_env

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_args_env.c args_env.o args_env \
	       cpp_mem.c mem.o \
	       cpp_vsprintf.c vsprintf.o \
	       arch_args_env.o
