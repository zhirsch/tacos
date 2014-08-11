# Builds the toolchain for building the tacos kernel.
# Based off of http://wiki.osdev.org/GCC_Cross-Compiler

set -ex

BINUTILS_VERSION=2.24
GCC_VERSION=4.9.1
NEWLIB_VERSION=2.1.0

ROOT=$(readlink -f $(dirname $0))
SOURCE=$ROOT/source
BUILD=$ROOT/build
PREFIX=$ROOT/install
TARGET=i686-pc-tacos

rm -rf $SOURCE
rm -rf $BUILD
rm -rf $PREFIX

mkdir -p $SOURCE
pushd $SOURCE
#curl http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.bz2 | tar jx
cat $ROOT/binutils-$BINUTILS_VERSION.tar.bz2 | tar jx
patch -d binutils-$BINUTILS_VERSION -p1 < ../binutils-$BINUTILS_VERSION.patch
#curl http://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.bz2 | tar jx
cat $ROOT/gcc-$GCC_VERSION.tar.bz2 | tar jx
patch -d gcc-$GCC_VERSION -p1 < ../gcc-$GCC_VERSION.patch
#curl ftp://sourceware.org/pub/newlib/newlib-$NEWLIB_VERSION.tar.gz | tar zx
cat $ROOT/newlib-$NEWLIB_VERSION.tar.gz | tar zx
patch -d newlib-$NEWLIB_VERSION -p1 < ../newlib-$NEWLIB_VERSION.patch
popd

pushd $SOURCE/newlib-$NEWLIB_VERSION
cd newlib/libc/sys
autoconf
cd tacos
ACLOCAL=aclocal-1.10 AUTOMAKE=automake-1.10 autoreconf
popd

mkdir $BUILD
cd $BUILD

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
PATH=$PATH:$PREFIX/bin $SOURCE/newlib-$NEWLIB_VERSION/configure \
    --target=$TARGET \
    --prefix=$PREFIX \
    --disable-nls
PATH=$PATH:$PREFIX/bin make -j4
PATH=$PATH:$PREFIX/bin make install
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
