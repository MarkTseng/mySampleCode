cmake_minimum_required(VERSION 3.13)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(DEFINED ENV{GCC_ARM_TOOLCHAIN})
    set(GCC_ARM_TOOLCHAIN $ENV{GCC_ARM_TOOLCHAIN})
else()
    set(GCC_ARM_TOOLCHAIN "/home/mark/mstar_workshop/toolchain/C4P/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf")
endif()

LIST(APPEND CMAKE_PROGRAM_PATH ${GCC_ARM_TOOLCHAIN})

# Specify the cross compiler
# The target triple needs to match the prefix of the binutils exactly
# (e.g. CMake looks for arm-none-eabi-ar)
SET(CMAKE_CROSSCOMPILING True)
set(CLANG_TARGET_TRIPLE arm-linux-gnueabihf)
set(GCC_ARM_TOOLCHAIN_PREFIX ${CLANG_TARGET_TRIPLE})
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_ASM_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})

set(LLVM_TABLEGEN "llvm-tblgen")
set(CLANG_TABLEGEN "clang-tblgen")
set(LLVM_DEFAULT_TARGET_TRIPLE "arm-linux-gnueabihf")
set(LLVM_TARGET_ARCH "ARM")
set(LLVM_TARGETS_TO_BUILD "ARM")

# Don't run the linker on compiler check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# C/C++ toolchain
set(GCC_ARM_SYSROOT "${GCC_ARM_TOOLCHAIN}/${GCC_ARM_TOOLCHAIN_PREFIX}/libc")
#set(CMAKE_CXX_FLAGS "-I${GCC_ARM_SYSROOT}/../include/c++/6.3.1/arm-linux-gnueabihf")
set(CMAKE_SYSROOT ${GCC_ARM_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${GCC_ARM_SYSROOT})

# Specify compiler flags
set(ARCH_FLAGS "--target=arm-linux-gnueabihf -march=armv7a --gcc-toolchain=${GCC_ARM_TOOLCHAIN} --sysroot=${GCC_ARM_TOOLCHAIN}/arm-linux-gnueabihf/libc -mfpu=neon-vfpv4 -mfloat-abi=hard -mno-unaligned-access")
set(CMAKE_C_FLAGS "-Wall ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -fno-rtti -fno-threadsafe-statics ${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
set(CMAKE_ASM_FLAGS "-Wall ${ARCH_FLAGS} -x assembler-with-cpp" CACHE STRING "Common flags for assembler")
#set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,-Map,kernel.map,--gc-sections -fuse-linker-plugin -Wl,--use-blx --specs=nano.specs --specs=nosys.specs" CACHE STRING "")
#set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# For libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
