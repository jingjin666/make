include ../toolchain.mk

#$(info MAKECMDGOALS:$(MAKECMDGOALS))
ifeq ($(MAKECMDGOALS),share)
default : share
CFLAGS += -fPIC
else
default : static
endif

#
# Things that, if they change, might affect everything
#
GLOBAL_DEPS	+= $(MAKEFILE_LIST)
#$(info GLOBAL_DEPS:$(GLOBAL_DEPS))

SRCS += $(wildcard src/ldso/*.c)
SRCS += $(wildcard src/crt/*.c)
SRCS += $(wildcard src/math/*.c)

TARGET 		=	libosx.a
OBJS		=	$(addsuffix .o,$(SRCS))
DEPS		=	$(addsuffix .d,$(SRCS))

SHARE_TARGET 	=	libosx-dl.so
SHARE_OBJS	=	$(addsuffix .s.o,$(SRCS))
SHARE_DEPS	=	$(addsuffix .s.d,$(SRCS))

DUMP		=	libosx.dump

INCLUDE_DIRS	+=	include
LDSCRPIT	=
EXTRALDFLAGS	+=
LIB_DIRS	+=
LIBS		+=

$(filter %.c.o,$(OBJS)): %.c.o : %.c $(GLOBAL_DEPS)
	$(call COMPILE,$<,$@)

$(filter %.cpp.o,$(OBJS)): %.cpp.o : %.cpp $(GLOBAL_DEPS)
	$(call COMPILEXX,$<,$@)

$(filter %.S.o,$(OBJS)): %.S.o: %.S $(GLOBAL_DEPS)
	$(call ASSEMBLE,$<,$@)

$(TARGET) : $(OBJS) $(GLOBAL_DEPS)
	$(call LINK_AR,$@,$(OBJS))
	$(call SYM_TO_ASS,$@,$(DUMP))
	readelf -a $@ > $(TARGET).re

static : clean $(TARGET)

$(SHARE_OBJS) : %.c.s.o : %.c
	$(call COMPILE,$<,$@)

$(SHARE_TARGET) : $(SHARE_OBJS)
	$(CC) -nostdlib -shared -Wl,-e,osx_dlstart,-z,max-page-size=4096 -o $@ $(SHARE_OBJS)
	$(call SYM_TO_ASS,$@,$(DUMP))
	readelf -a $@ > $(SHARE_TARGET).re

share : clean $(SHARE_TARGET)

.PHONY : static share clean

clean :
	@rm -rf $(TARGET) $(OBJS) $(DEPS) $(SHARE_TARGET) $(SHARE_OBJS) $(SHARE_DEPS) $(SHARE_TARGET).re $(TARGET).re $(DUMP)
