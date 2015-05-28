.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

conf_ini:conf_ini.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. ../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_mem.c -o mem.o
	cpp -Wall -Wextra -I../../.. ../../utils/ascii/string/vsprintf.c \
		-o cpp_vsprintf.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_vsprintf.c -o vsprintf.o
	as ../../arch/args_env.s -o args_env.o
	cpp -Wall -Wextra -I../../.. \
		../../utils/ascii/block/conv/decimal/decimal.c \
		-o cpp_decimal.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_decimal.c -o decimal.o
	as ../../arch/args_env.s -o args_env.o
	cpp -Wall -Wextra -I../../.. conf_ini.c -o cpp_conf_ini.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -c cpp_conf_ini.c -o conf_ini.o
	ld -O -nostdlib -Bstatic --strip-all conf_ini.o args_env.o string.o \
		mem.o vsprintf.o decimal.o $$(gcc -print-libgcc-file-name) \
		--output conf_ini

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_conf_ini.c conf_ini.o conf_ini \
	       cpp_mem.c mem.o \
	       cpp_decimal.c decimal.o \
	       cpp_vsprintf.c vsprintf.o \
	       args_env.o

