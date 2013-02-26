.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native 

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

thread:thread.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. thread.c -o cpp_thread.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_thread.c \
		-o thread.s
	as thread.s -o thread.o
	ld -O -nostdlib -Bstatic --strip-all thread.o \
		$$(gcc -print-libgcc-file-name) --output thread

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_thread.c thread.s thread.o thread
