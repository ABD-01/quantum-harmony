# cmake/mingw-gcc-toolchain.cmake
#
# Copyright (c) 2024 Accolade Electronics Pvt. Ltd.
# 
# Date:     May 29, 2024
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# 
# Brief:    This file holds GNU GCC toolchain settings, paths, etc.
#           Sets various compiler flags as per BUILD_TYPE

set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_PROCESSOR "x86_64")

set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
set(CMAKE_ASM_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")
set(CMAKE_AR "C:/msys64/ucrt64/bin/ar.exe")
set(CMAKE_OBJCOPY "C:/msys64/ucrt64/bin/objcopy.exe")
set(CMAKE_OBJDUMP "C:/msys64/ucrt64/bin/objdump.exe")
set(CMAKE_SIZE "C:/msys64/ucrt64/bin/size.exe")
set(CMAKE_STRIP "C:/msys64/ucrt64/bin/strip.exe")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")