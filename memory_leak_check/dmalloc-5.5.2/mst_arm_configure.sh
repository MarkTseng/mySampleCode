make clean
rm -rf DMALLOC
./configure --prefix=$PWD/DMALLOC --host=arm-linux-gnueabihf CFLAGS="-U_FORTIFY_SOURCE -fPIC" --enable-threads --enable-shlib 
make 
make shlib threads
make install
tar cfJ DMALLOC.tar.xz DMALLOC

