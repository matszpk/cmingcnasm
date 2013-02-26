.PHONY:clean

#does remove some useless ELF sections
OPTIM=-fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm \
-Ofast -march=native 

ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

uevents_monitor:uevents_monitor.c ../../../ulinux/arch
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
	cpp -Wall -Wextra -I../../.. uevents_monitor.c -o cpp_uevents_monitor.c
	gcc -Wall -Wextra -std=gnu99 $(OPTIM) -fverbose-asm -S \
		cpp_uevents_monitor.c -o uevents_monitor.s
	as uevents_monitor.s -o uevents_monitor.o
	ld -O -nostdlib -Bstatic --strip-all uevents_monitor.o string.o mem.o \
		vsprintf.o $$(gcc -print-libgcc-file-name) \
		--output uevents_monitor

../../../ulinux/arch:
	ln -s archs/$(ARCH) ../../../ulinux/arch

clean:
	-rm -f ../../../ulinux/arch
	-rm -f \
		cpp_uevents_monitor.c uevents_monitor.s uevents_monitor.o \
			uevents_monitor \
		cpp_mem.c mem.s mem.o \
		cpp_string.c string.s string.o \
		cpp_vsprintf.c vsprintf.s vsprintf.o
