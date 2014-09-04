# The versions of the tools and libraries to build.
BINUTILS_VERSION = 2.24
DASH_VERSION     = 0.5.7
GCC_VERSION      = 4.9.1
NEWLIB_VERSION   = 2.1.0

# The root directory containing this makefile.
ROOT := $(shell dirname $(shell readlink -f $(lastword $(MAKEFILE_LIST))))

# The directory to build the packages in.
BUILD = $(ROOT)/build
# The directory to install the packages in.
PREFIX = $(ROOT)/install
# The directory containing the sources.
SOURCE = $(ROOT)/source
# The directory containing build log files.
LOGS = $(ROOT)/logs

export PATH := $(PATH):$(PREFIX)/bin

# The target for user space.
TARGET = i686-pc-tacos
# The target for kernel space.
KERNEL_TARGET = i686-pc-tacoskernel

define _run
	@mkdir -p $(LOGS) $(BUILD)/$@
	@(cd $(BUILD)/$@ && $(1)) >> $(LOGS)/$@.log 2>&1
endef

define configure
	@echo "CONFIGURE $@"
	$(call _run, $(1))
endef
define build
	@echo "BUILD     $@"
	$(call _run, $(1))
endef
define install
	@echo "INSTALL   $@"
	$(call _run, $(1))
endef

all: kernel user

clean:
	rm -rf $(BUILD) $(PREFIX)

kernel: binutils-kernel gcc-kernel

user: binutils gcc newlib dash

binutils-kernel:
	$(call configure, $(SOURCE)/binutils-$(BINUTILS_VERSION)/configure \
	    --target=$(KERNEL_TARGET) \
	    --prefix=$(PREFIX) \
	    --disable-nls)
	$(call build, make -j4)
	$(call install, make install-strip)

gcc-kernel: binutils-kernel
	$(call configure, $(SOURCE)/gcc-$(GCC_VERSION)/configure \
	    --target=$(KERNEL_TARGET) \
	    --prefix=$(PREFIX) \
	    --disable-nls \
	    --enable-languages=c)
	$(call build, make -j4 all-gcc all-target-libgcc)
	$(call install, make install-strip-gcc install-strip-target-libgcc)

binutils:
	$(call configure, $(SOURCE)/binutils-$(BINUTILS_VERSION)/configure \
	    --target=$(TARGET) \
	    --prefix=$(PREFIX) \
	    --disable-nls)
	$(call build, make -j4)
	$(call install, make install-strip)

gcc: gcc-bootstrap binutils newlib
	$(call configure, $(SOURCE)/gcc-$(GCC_VERSION)/configure \
	    --target=$(TARGET) \
	    --prefix=$(PREFIX) \
	    --disable-nls \
	    --with-newlib \
	    --enable-languages=c)
	$(call build, make -j4)
	$(call install, make install-strip)

gcc-bootstrap: binutils
	$(call configure, $(SOURCE)/gcc-$(GCC_VERSION)/configure \
	    --target=$(TARGET) \
	    --prefix=$(PREFIX) \
	    --disable-nls \
	    --without-headers \
	    --with-newlib \
	    --enable-languages=c)
	$(call build, make -j4 all-gcc all-target-libgcc)
	$(call install, make install-strip-gcc install-strip-target-libgcc)

newlib: gcc-bootstrap
	$(call configure, $(SOURCE)/newlib-$(NEWLIB_VERSION)/configure \
	    --target=$(TARGET) \
	    --prefix=$(PREFIX) \
	    CFLAGS='-fno-omit-frame-pointer -g3 -D_I386MACH_ALLOW_HW_INTERRUPTS')
	$(call build, make -j4)
	$(call install, make install)

dash: gcc newlib
	$(call configure, $(SOURCE)/dash-$(DASH_VERSION)/configure \
	    --host=$(TARGET) \
	    --prefix=$(PREFIX) \
	    CFLAGS='-fno-omit-frame-pointer -g3')
	$(call build, make -j4)
	$(call install, make install)
