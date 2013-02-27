include conf.mk

.PHONY:all dirs clean help

DIRS=\
$(CPP_DIR) \
$(OBJ_DIR) \
$(BIN_DIR) \
$(LIB_DIR)

#-------------------------------------------------------------------------------
LIBCMINGCNASM_ULINUX_OBJS=\
$(OBJ_DIR)/decimal.o \
$(OBJ_DIR)/binary.o \
$(OBJ_DIR)/hexadecimal.o \
$(OBJ_DIR)/vsprintf.o \
$(OBJ_DIR)/string.o


LIBCMINGCNASM_OBJS=\
$(OBJ_DIR)/src_parse.o \
$(OBJ_DIR)/m_emit.o \
$(OBJ_DIR)/msgs.o \
$(OBJ_DIR)/libcmingcnasm.o \
$(LIBCMINGCNASM_ULINUX_OBJS)

CMINGCNASM_ULINUX_OBJS=\
$(OBJ_DIR)/args_env.o \
$(OBJ_DIR)/mem.o \
$(OBJ_DIR)/vsprintf.o \
$(OBJ_DIR)/string.o

CMINGCNASM_OBJS=\
$(OBJ_DIR)/cmingcnasm.o \
$(CMINGCNASM_ULINUX_OBJS)
#-------------------------------------------------------------------------------
LIBCMINGCNDIS_ULINUX_OBJS=

LIBCMINGCNDIS_OBJS=\
$(OBJ_DIR)/msgs.o \
$(OBJ_DIR)/libcmingcndis.o \
$(LIBCMINGCNDIS_ULINUX_OBJS)

CMINGCNDIS_ULINUX_OBJS=\
$(OBJ_DIR)/args_env.o \
$(OBJ_DIR)/mem.o \
$(OBJ_DIR)/vsprintf.o \
$(OBJ_DIR)/string.o

CMINGCNDIS_OBJS=\
$(OBJ_DIR)/cmingcndis.o \
$(CMINGCNDIS_ULINUX_OBJS)
#-------------------------------------------------------------------------------
#*******************************************************************************
#*******************************************************************************
help:
	@echo "targets are 'all', 'help'(this output), 'clean'"
	@echo -e "you can configure the build with the following variables:\\n\
	SYSROOT, ARCH, CPP, CC, LD, CPPFLAGS, CFLAGS, LDFLAGS...\\n\
	(you can tune the conf.mk file)"

all:dirs ulinux/arch $(BIN_DIR)/cmingcnasm $(BIN_DIR)/cmingcndis

dirs:$(DIRS)
$(DIRS):
	-mkdir -p $@

ulinux/arch:
	ln -s archs/$(ARCH) ulinux/arch

clean:
	-rm -f ulinux/arch
	-rm -Rf $(BUILD_DIR)
#*******************************************************************************

#*******************************************************************************
#ulinux objects
$(OBJ_DIR)/args_env.o:ulinux/arch/args_env.s
	$(AS) $(ASFLAGS) $< -o $@

$(CPP_DIR)/mem.c:ulinux/utils/mem.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/mem.o:$(CPP_DIR)/mem.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/vsprintf.c:ulinux/utils/ascii/string/vsprintf.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/vsprintf.o:$(CPP_DIR)/vsprintf.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/string.c:ulinux/utils/ascii/string/string.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/string.o:$(CPP_DIR)/string.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/decimal.c:ulinux/utils/ascii/block/conv/decimal/decimal.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/decimal.o:$(CPP_DIR)/decimal.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/hexadecimal.c:ulinux/utils/ascii/block/conv/hexadecimal/hexadecimal.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/hexadecimal.o:$(CPP_DIR)/hexadecimal.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/binary.c:ulinux/utils/ascii/block/conv/binary/binary.c
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/binary.o:$(CPP_DIR)/binary.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@
#*******************************************************************************

#*******************************************************************************
#libcmingcnasm objects
$(CPP_DIR)/src_parse.c:src_parse.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/src_parse.o:$(CPP_DIR)/src_parse.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/m_emit.c:m_emit.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/m_emit.o:$(CPP_DIR)/m_emit.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/msgs.c:msgs.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/msgs.o:$(CPP_DIR)/msgs.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

$(CPP_DIR)/libcmingcnasm.c:libcmingcnasm.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/libcmingcnasm.o:$(CPP_DIR)/libcmingcnasm.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@
#*******************************************************************************

#*******************************************************************************
#cmingcnasm objects
$(CPP_DIR)/cmingcnasm.c:cmingcnasm.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/cmingcnasm.o:$(CPP_DIR)/cmingcnasm.c
	$(CC) -c $(CFLAGS) $< -o $@
#*******************************************************************************

#*******************************************************************************
#libcmingcndis objects
$(CPP_DIR)/libcmingcndis.c:libcmingcndis.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/libcmingcndis.o:$(CPP_DIR)/libcmingcndis.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@
#*******************************************************************************

#*******************************************************************************
#cmingcndis objects
$(CPP_DIR)/cmingcndis.c:cmingcndis.c 
	$(CPP) $(CPPFLAGS) $< -o $@
$(OBJ_DIR)/cmingcndis.o:$(CPP_DIR)/cmingcndis.c
	$(CC) -c $(CFLAGS) $< -o $@
#*******************************************************************************

$(LIB_DIR)/libcmingcnasm.so:$(LIBCMINGCNASM_OBJS)
	$(LD) $(LDFLAGS_LIB) $(LIBCMINGCNASM_OBJS) -o $@

$(BIN_DIR)/cmingcnasm:$(CMINGCNASM_OBJS) $(LIB_DIR)/libcmingcnasm.so
	$(LD) $(LDFLAGS_BIN) $(CMINGCNASM_OBJS) -lcmingcnasm \
          -o $@

$(LIB_DIR)/libcmingcndis.so:$(LIBCMINGCNDIS_OBJS)
	$(LD) $(LDFLAGS_LIB) $(LIBCMINGCNDIS_OBJS) -o $@

$(BIN_DIR)/cmingcndis:$(CMINGCNDIS_OBJS) $(LIB_DIR)/libcmingcndis.so
	$(LD) $(LDFLAGS_BIN) $(CMINGCNDIS_OBJS) -lcmingcndis \
          -o $@
