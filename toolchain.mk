# Cross compile path
CROSSDEV   = aarch64-none-elf

# Toolchain commands
#
CC 			=	$(CROSSDEV)-gcc
CXX 			=	$(CROSSDEV)-g++
LD			=	$(CROSSDEV)-ld
AR			=	$(CROSSDEV)-ar
NM			=	$(CROSSDEV)-nm
OBJCOPY 		=	$(CROSSDEV)-objcopy
OBJDUMP 		=	$(CROSSDEV)-objdump

# Base CPU architecture flags
#
ARCHCPUFLAGS		=	-march=armv8-a

# optimisation flags
#
ARCHOPTIMIZATION 	=	-O3 \
				-g3 \
				-fno-strict-aliasing \
				-fno-strength-reduce \
				-fomit-frame-pointer \
				-funsafe-math-optimizations \
				-fno-builtin-printf \
				-ffunction-sections \
				-fdata-sections

# Language-specific flags
#
ARCHCFLAGS		=	-std=gnu99 \
				-nostdlib \
				-nostdinc \
				-fno-common \
				-fno-builtin

ARCHCXXFLAGS		=	-std=gnu++17 \
				-nostdlib \
				-fno-builtin \
				-fno-exceptions \
				-fno-rtti \
				-fno-threadsafe-statics

EXTRADEFINES		=

EXTRACFLAGS		=

EXTRACXXFLAGS		=

# Flags we pass to the C compiler
#
CFLAGS			=	$(ARCHCFLAGS) \
				$(ARCHCPUFLAGS) \
				$(EXTRADEFINES) \
				$(EXTRACFLAGS) \
				$(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the C++ compiler
#
CXXFLAGS		=	$(ARCHCXXFLAGS) \
				$(ARCHCPUFLAGS) \
				$(EXTRADEFINES) \
				$(EXTRACXXFLAGS) \
				$(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the assembler
#
AFLAGS			=	$(CFLAGS) -D__ASSEMBLY__ \
				$(EXTRADEFINES) \
				$(EXTRAAFLAGS)

EXTRALDFLAGS		=

# Flags we pass to the linker
#
LDFLAGS			+=	--warn-common \
				--gc-sections \
				$(EXTRALDFLAGS) \
				$(addprefix -T,$(LDSCRIPT)) \
				$(addprefix -L,$(LIB_DIRS))

# Compiler support library
#
LIBGCC			:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-libgcc-file-name)
# EXTRA_LIBS		+=	$(LIBGCC)

# pull in *just* libnosys from the toolchain
LIBNOSYS		:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libnosys.a)
# EXTRA_LIBS		+=	$(LIBNOSYS)

# pull in *just* libm from the toolchain
LIBM			:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libm.a)
#EXTRA_LIBS		+=	$(LIBM)

# pull in *just* libc from the toolchain
LIBC			:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libc.a)
#EXTRA_LIBS		+=	$(LIBC)

# pull in *just* libsupcxx from the toolchain
LIBSUPCXX		:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libsupc++.a)
# EXTRA_LIBS		+=	$(LIBSUPCXX)

# pull in *just* libstdcxx from the toolchain
LIBSTDCXX		:=	$(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libstdc++.a)
# EXTRA_LIBS		+=	$(LIBSTDCXX)

# Compile C source $1 to object $2
# as a side-effect, generate a dependency file
#
define COMPILE
	@echo "CC:	$1"
	@mkdir -p $(dir $2)
	@$(CC) -MD -c $(CFLAGS) $(abspath $1) -o $2
endef

# Compile C++ source $1 to $2
# as a side-effect, generate a dependency file
#
define COMPILEXX
	@echo "CXX:	$1"
	@mkdir -p $(dir $2)
	@$(CXX) -MD -c $(CXXFLAGS) $(abspath $1) -o $2
endef

# Assemble $1 into $2
#
define ASSEMBLE
	@echo "AS:	$1"
	@mkdir -p $(dir $2)
	$(CC) -c $(AFLAGS) $(abspath $1) -o $2
endef

# Link the objects in $2 into the binary $1
#
define LINK
	@echo "LINK:	$1"
	@mkdir -p $(dir $1)
	$(LD) $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) --end-group
endef

# AR the objects in $2 into the static binary $1
#
define LINK_AR
	@echo "AR:	$1"
	@mkdir -p $(dir $1)
	@$(AR) cr $1 $2
endef

# Convert $1 from a linked object to a raw binary in $2
#
define SYM_TO_ASS
	@echo "OBJDUMP:	$2"
	@mkdir -p $(dir $2)
	$(OBJDUMP) -D -Slx $1 > $2
endef

# Convert $1 from a linked object to a raw binary in $2
#
define SYM_TO_BIN
	@echo "BIN:	$2"
	@mkdir -p $(dir $2)
	$(OBJCOPY) -O binary $1 $2
endef
