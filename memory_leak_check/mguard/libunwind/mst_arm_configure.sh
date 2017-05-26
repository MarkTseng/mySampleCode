#!/bin/bash

autoreconf -i

./configure --prefix=$(pwd)/build --enable-static --enable-shared --with-pic CFLAGS="-U_FORTIFY_SOURCE" CC=arm-linux-gnueabihf-gcc --host=arm-linux

