.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

epoll_signalfd:epoll_signalfd.c ../../../ulinux/arch
	cpp -Wall -Wextra -I../../.. ../../utils/mem.c -o cpp_mem.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S cpp_mem.c -o mem.s
	as mem.s -o mem.o
	cpp -Wall -Wextra -I../../.. epoll_signalfd.c \
		-o cpp_epoll_signalfd.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S \
		cpp_epoll_signalfd.c -o epoll_signalfd.s
	as epoll_signalfd.s -o epoll_signalfd.o
	ld -O -nostdlib -Bstatic --strip-all mem.o epoll_signalfd.o \
		$$(gcc -print-libgcc-file-name) --output epoll_signalfd

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f cpp_epoll_signalfd.c epoll_signalfd.s epoll_signalfd.o \
		epoll_signalfd cpp_mem.c mem.s mem.o
