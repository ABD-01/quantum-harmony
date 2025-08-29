#!/bin/bash

# Initialize build failure flag.
build_failed=0

# Function that returns original directory and clean-up.
function cleanup() {
    echo "***** Clean-up *****"
    cd ..
    rm -rf "${BUILD_DIR}"
    if [ "$build_failed" -eq -1 ]; then
        echo "***** CMake configure failed. *****"
    elif [ "$build_failed" -eq -2 ]; then
        echo "***** CMake build failed. *****"
    elif [ "$build_failed" -eq -3 ]; then
        echo "***** CMake package failed. *****"
    else
        echo "***** Graceful finish, have a good day. *****"
    fi
    echo -e "*********************************************\n"
    exit 0
}

# CMake-based build script for ASL library
echo "***** CMake Build System for ASL *****"

# Check if TARGET_PLATFORM is provided as argument.
if [ "$1" == "default" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "mc" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "nxp" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "st" ]; then
    TARGET_PLATFORM="$1"
elif [ "$1" == "ti" ]; then
    TARGET_PLATFORM="$1"
else
    TARGET_PLATFORM="default"
fi
echo "Building for platform: ${TARGET_PLATFORM}"

# Setup build environment.
BUILD_DIR="build_cmake_${TARGET_PLATFORM}"
if [ -d "${BUILD_DIR}" ]; then
    echo "Cleaning previous build directory..."
    rm -rf "${BUILD_DIR}"
fi
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Prepare the toolchain file.
CMAKE_TOOLCHAIN_FILE_RELATIVE="$(pwd)/../../toolchains/${TARGET_PLATFORM}/${TARGET_PLATFORM}_toolchain.cmake"
CMAKE_TOOLCHAIN_FILE=$(realpath "$CMAKE_TOOLCHAIN_FILE_RELATIVE")

# Configure with CMake, using 'MinGW Makefiles' generator for Windows compatibility.
echo "***** CMake Configuration *****"
cmake -G "MinGW Makefiles" \
    -DTARGET_PLATFORM=${TARGET_PLATFORM} \
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} \
    ..
if [ $? -ne 0 ]; then
    build_failed=-1
    cleanup
fi

# Build the project.
echo "***** CMake Build *****"
cmake --build .
if [ $? -ne 0 ]; then
    build_failed=-2
    cleanup
fi

# Create package using CPack (explicit target)
echo "***** CPack Package *****"
cmake --build . --target package
if [ $? -ne 0 ]; then
    build_failed=-3
    cleanup
fi

# Copy produced package into ./../../libs/asl
EXPORT_PKG_NAME="${TARGET_PLATFORM}_libasl.tar.gz"
EXPORT_PKG_PATH_RELATIVE="./../../libs/asl"
mkdir -p "$EXPORT_PKG_PATH_RELATIVE"
EXPORT_PKG_PATH=$(realpath "$EXPORT_PKG_PATH_RELATIVE")
echo "Copying package ${EXPORT_PKG_NAME} to ${EXPORT_PKG_PATH}"
mkdir -p "${EXPORT_PKG_PATH}"
cp "${EXPORT_PKG_NAME}" "${EXPORT_PKG_PATH}"

# Normal exit.
cleanup
