make clean
rm -rf DMALLOC
./configure --prefix=$PWD/DMALLOC --host=arm-linux-gnueabihf CFLAGS="-fstack-protector -U_FORTIFY_SOURCE -fPIC -O2" LDFLAGS="-ldl " --enable-threads --enable-shlib 
make 
make shlib threads 
make installincs installthsl 
tar cfJ DMALLOC.tar.xz DMALLOC
cp -a DMALLOC.tar.xz ~/mstar_workshop/IPCM/nexus/arm-linux-gnueabihf-4.8.3-201404/
cp -a DMALLOC/lib/* ~/nfs/lib
sync
