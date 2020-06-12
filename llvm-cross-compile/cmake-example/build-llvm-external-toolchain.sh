rm -rf build
mkdir build; cd build
#TOOLCHAIN_FILE=$PWD/../linux-arm-llvm-miltilib.cmake
TOOLCHAIN_FILE=$PWD/../linux-arm-llvm.cmake
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE ..
#make
ninja
