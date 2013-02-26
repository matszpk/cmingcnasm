#*******************************************************************************
#this code is protected by the GNU affero GPLv3
#author:Sylvain BERTRAND <sylvain.bertrand AT gmail dot com>
#                        <digital.ragnarok AT gmail dot com>
#*******************************************************************************
export
QUIET?=
DEBUG?=
SYSROOT?=
ARCH?=$(shell uname -m | sed -e s/i.86/x86/ -e s/parisc64/parisc/ \
-e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/sh.*/sh/)

BUILD_DIR?=build
BIN_DIR?=$(BUILD_DIR)/bin
LIB_DIR?=$(BUILD_DIR)/lib
OBJ_DIR?=$(BUILD_DIR)/obj
CPP_DIR?=$(BUILD_DIR)/cpp

LD:=gcc
CC:=gcc
CPP:=cpp
AS:=as

CPPFLAGS?=-Wall -Wextra -I./ -I../
ifndef DEBUG
CFLAGS?=-Wall -Wextra -std=gnu99 -Ofast -march=native
LDFLAGS_BIN?=-nostdlib -Wl,--strip-all,-O,10 -L$(LIB_DIR)
LDFLAGS_LIB?=-shared -nostdlib -Wl,--strip-all,-O,10
ASFLAGS?=
else
CFLAGS?=-Wall -Wextra -std=gnu99 -ggdb3 -march=native
LDFLAGS_BIN?=-nostdlib -L$(LIB_DIR)
LDFLAGS_LIB?=-nostdlib
ASFLAGS?=
endif

ifdef QUIET
CPPFLAGS+= -DQUIET
endif
