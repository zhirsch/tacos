# Builds the toolchain for building the tacos kernel.

set -ex

BINUTILS_VERSION=2.24
GCC_VERSION=4.9.1
NEWLIB_VERSION=2.1.0
DASH_VERSION=0.5.7

ROOT=$(readlink -f $(dirname $0))
SOURCE=$ROOT/source
BUILD=$ROOT/build
PREFIX=$ROOT/install
TARGET=i686-pc-tacos
KERNEL_TARGET=${TARGET}kernel

rm -rf $BUILD
rm -rf $PREFIX

export PATH=$PATH:$PREFIX/bin

mkdir $BUILD
cd $BUILD

# --- Build once for the kernel.
# Binutils
mkdir binutils-kernel
pushd binutils-kernel
$SOURCE/binutils-$BINUTILS_VERSION/configure \
    --target=$KERNEL_TARGET \
    --prefix=$PREFIX \
    --disable-nls
make -j4
make install-strip
popd
# GCC
mkdir gcc-kernel
pushd gcc-kernel
$SOURCE/gcc-$GCC_VERSION/configure \
    --target=$KERNEL_TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --enable-languages=c
make -j4 all-gcc all-target-libgcc
make install-strip-gcc install-strip-target-libgcc
popd

# --- Build again for user space.
# Binutils
mkdir binutils
pushd binutils
$SOURCE/binutils-$BINUTILS_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls
make -j4
make install-strip
popd
# GCC bootstrap
mkdir gcc
pushd gcc
$SOURCE/gcc-$GCC_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --without-headers \
    --with-newlib \
    --enable-languages=c
make -j4 all-gcc all-target-libgcc
make install-strip-gcc install-strip-target-libgcc
popd
# Newlib
mkdir newlib
pushd newlib
$SOURCE/newlib-$NEWLIB_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    CFLAGS='-fno-omit-frame-pointer -g3 -D_I386MACH_ALLOW_HW_INTERRUPTS'
make -j4
make install
popd
# GCC full
pushd gcc
$SOURCE/gcc-$GCC_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls \
    --with-newlib \
    --enable-languages=c
make -j4
make install-strip
popd

# --- Build user space programs.
# Dash
mkdir dash
pushd dash
$SOURCE/dash-$DASH_VERSION/configure \
    --host=$TARGET \
    --prefix=$PREFIX \
    CFLAGS='-fno-omit-frame-pointer -g3'
make -j4
make install
popd
