.PHONY:clean all help

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native -fPIC

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

help:
	@echo "to test use command line:LD_LIBRARY_PATH=./ (you linux loader path)\
use_os"

all:so.so use_so

so.so:so.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. ../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_mem.c -o mem.s
	as mem.s -o mem.o
	cpp -Wall -Wextra -I../../.. ../../utils/ascii/string/vsprintf.c \
		-o cpp_vsprintf.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_vsprintf.c \
		-o vsprintf.s
	as vsprintf.s -o vsprintf.o
	cpp -Wall -Wextra -I../../.. ../../utils/ascii/string/string.c \
		-o cpp_string.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_string.c \
		-o string.s
	as string.s -o string.o
	cpp -Wall -Wextra -I../../.. so.c -o cpp_so.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_so.c \
		-o so.s
	as so.s -o so.o
	ld -O -nostdlib -shared so.o string.o mem.o \
		vsprintf.o $$(gcc -print-libgcc-file-name) --output so.so

use_so:use_so.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. use_so.c -o cpp_use_so.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_use_so.c \
		-o use_so.s
	as use_so.s -o use_so.o
	ld -O -nostdlib use_so.o so.so \
		$$(gcc -print-libgcc-file-name) --output use_so

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_so.c so.s so.o so.so \
		cpp_use_so.c use_so.s use_so.o use_so \
		cpp_mem.c mem.s mem.o \
		cpp_string.c string.s string.o \
		cpp_vsprintf.c vsprintf.s vsprintf.o
