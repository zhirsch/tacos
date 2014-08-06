# Builds the toolchain for building the tacos kernel.
# Based off of http://wiki.osdev.org/GCC_Cross-Compiler

set -ex

BINUTILS_VERSION=2.24
GCC_VERSION=4.9.1

PREFIX=$(readlink -f $(dirname $0))
TARGET=i686-elf

cd $PREFIX

# Binutils
rm -rf binutils-$BINUTILS_VERSION
tar jxf binutils-$BINUTILS_VERSION.tar.bz2
rm -rf build-binutils
mkdir -p build-binutils
pushd build-binutils
../binutils-$BINUTILS_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls
make -j4
make install-strip
popd

# GCC
rm -rf gcc-$GCC_VERSION
tar jxf gcc-$GCC_VERSION.tar.bz2
rm -rf build-gcc
mkdir -p build-gcc
pushd build-gcc
../gcc-$GCC_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --enable-languages=c \
    --without-headers
make -j4 all-gcc
make -j4 all-target-libgcc
make install-strip-gcc
make install-strip-target-libgcc
popd
