#!/bin/bash

# Create and enter the build directory.
echo "***** CMake Build System for ASL library *****"
rm -rf build
mkdir build
cd build

# Set top level macros ATP_ROOT, TARGET_PLATFORM and CMAKE_TOOLCHAIN_FILE.
ATP_ROOT="/d/projects/atp"
if [ "$1" == "mc" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "nxp" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "quectel" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "renesas" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "st" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "telit" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "ti" ]; then
    TARGET_PLATFORM="$1"
else
    TARGET_PLATFORM="default"
fi
CMAKE_TOOLCHAIN_FILE="${ATP_ROOT}/toolchains/${TARGET_PLATFORM}/${TARGET_PLATFORM}_toolchain.cmake"

# Configure with CMake, using 'MinGW Makefiles' generator for Windows compatibility.
echo "***** CMake Configuration *****"
cmake -Wno-dev -G "MinGW Makefiles" \
    -DATP_ROOT=${ATP_ROOT} \
    -DTARGET_PLATFORM=${TARGET_PLATFORM} \
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    ..

# Build the project.
echo "***** CMake Build *****"
cmake --build . --parallel 4

# Create package using CPack and then publish.
echo "***** CPack Package *****"
cmake --build . --target package_and_publish
