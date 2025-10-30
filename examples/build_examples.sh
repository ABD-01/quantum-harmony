#!/bin/bash

# Create and enter the build directory.
echo "***** CMake Build System for ASL library examples *****"
rm -rf build
mkdir build
cd build

# Set top level macros ATP_ROOT, TARGET_PLATFORM and CMAKE_TOOLCHAIN_FILE.
ATP_ROOT="/d/projects/atp"
TARGET_PLATFORM="default"
CMAKE_TOOLCHAIN_FILE="${ATP_ROOT}/toolchains/${TARGET_PLATFORM}/${TARGET_PLATFORM}_toolchain.cmake"

# Configure with CMake, using 'MinGW Makefiles' generator for Windows compatibility.
echo "***** CMake Configuration *****"
cmake -Wno-dev -G "MinGW Makefiles" \
    -DATP_ROOT=${ATP_ROOT} \
    -DTARGET_PLATFORM=${TARGET_PLATFORM} \
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} \
    ..

# Build the project.
echo "***** CMake Build *****"
cmake --build .
