#!/bin/bash

# Exit on any error
set -e

# Handle clean command
if [ "$1" = "clean" ]; then
    rm -rf "$BUILD_DIR"
    echo "Clean completed."
    exit 0
fi

# Source CSDTK4 environment variables
echo "Sourcing CSDTK4 environment variables..."
source "C:/CSDTK4/CSDTKvars.sh"

# Set build directory and type
BUILD_DIR="build"
BUILD_TYPE="Release"  

echo "Build directory: $BUILD_DIR"
echo "Build type: $BUILD_TYPE"

# Create and enter build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir "$BUILD_DIR"
fi
cd "$BUILD_DIR"

MAKE_PROGRAM="C:/CSDTK4/make/make.exe"
# Configure with CMake
echo "Configuring project with CMake..."
cmake \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/mips-toolchain.cmake \
    -DCMAKE_MAKE_PROGRAM="$MAKE_PROGRAM" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -G "Unix Makefiles" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    ..

# Build the project
echo "Building project..."
# Option 1: Use cmake --build (preferred for consistency)
cmake --build .

# Option 2: Use make directly 
# $MAKE_PROGRAM

echo "Build completed successfully"