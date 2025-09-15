include toolchain.mk

default: dltest

#
# Things that, if they change, might affect everything
#
GLOBAL_DEPS	+=	$(MAKEFILE_LIST)
# $(info GLOBAL_DEPS:$(GLOBAL_DEPS))

SRCS		=	dl_test.c
OBJS		=	$(addsuffix .o,$(SRCS))
DEPS		=	$(addsuffix .d,$(SRCS)) *.map

TARGET 		=	dl_test
DUMP		=	$(TARGET).dump

LDSCRIPT	=	./linker.lds
EXTRACFLAGS	+=
EXTRALDFLAGS	+=	-z max-page-size=4096

INCLUDE_DIRS	+=	../musl-libc/include
LIB_DIRS	+=	../musl-libc/lib

ifeq ($(MAKECMDGOALS),share)
EXTRALDFLAGS	+=	--dynamic-linker=libc.so
else
EXTRALDFLAGS	+=	-static
endif

LIBS		+=	-lc

EXTRA_LIBS	+=	../musl-libc/lib/crt1.o
EXTRA_LIBS	+=	../musl-libc/lib/crti.o
EXTRA_LIBS	+=	../musl-libc/lib/crtn.o

$(filter %.c.o,$(OBJS)): %.c.o : %.c $(GLOBAL_DEPS)
	$(call COMPILE,$<,$@)

$(filter %.cpp.o,$(OBJS)): %.cpp.o : %.cpp $(GLOBAL_DEPS)
	$(call COMPILEXX,$<,$@)

$(filter %.S.o,$(OBJS)): %.S.o: %.S $(GLOBAL_DEPS)
	$(call ASSEMBLE,$<,$@)

$(TARGET) : $(OBJS) $(GLOBAL_DEPS)
	$(call LINK,$@,$(OBJS))
	$(call SYM_TO_ASS,$@,$(DUMP))
	readelf -a $@ > $(TARGET).re

dltest : clean $(TARGET)

share : dltest

.PHONY : share clean

clean :
	rm -rf $(TARGET) $(OBJS) $(DEPS) $(DUMP) $(TARGET).re

