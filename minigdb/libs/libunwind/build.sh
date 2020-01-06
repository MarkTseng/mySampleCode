#!/bin/bash
make distclean
./configure --enable-static --enable-shared --with-pic --prefix=$PWD/LIBUNWIND_ARM CFLAGS="-U_FORTIFY_SOURCE" CC=arm-linux-gnueabihf-gcc --host=arm-linux
make
rm -rf LIBUNWIND_ARM
make install
cp -a LIBUNWIND_ARM/lib/* ~/nfs/LIBUNWIND_ARM/lib/;sync
