#!/bin/bash

# Build script for ASL LIFO Basic Example
#
# Copyright: Accolade Electronics Pvt Ltd, 2025-26
# All Rights Reserved
# UNPUBLISHED, LICENSED SOFTWARE.
# Accolade Electronics, Pune
# CONFIDENTIAL AND PROPRIETARY INFORMATION
# WHICH IS THE PROPERTY OF M/s Accolade Electronics.
#
# Date: 12 August 2025
# Brief: Build script for LIFO basic example with file simulation

set -e  # Exit on any error

# Script directory.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Input paths.
ASL_LIB="$SCRIPT_DIR/../../../libs/asl/default_libasl.tar.gz"

# Output directory.
BUILD_DIR="$SCRIPT_DIR/build"
mkdir -p "$BUILD_DIR"

# Compiler settings.
export CFLAGS="-Wall -Werror -std=c99 -pedantic -g"

# Colors for output.
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output.
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}
print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}
print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists.
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Main script execution.
print_info "========================================"
print_info "Starting ASL LIFO Basic Example build..."
print_info "========================================"

# Check for compiler.
print_info "Checking build environment..."
if ! command_exists gcc; then
    print_error "gcc compiler not found. Please install gcc."
    exit 1
fi
if ! command_exists tar; then
    print_error "tar utility not found. Please install tar."
    exit 1
fi
print_success "Build tools verified."

# Check for core library and it's dependency.
print_info "Checking for core libraries..."
if [ ! -f "$ASL_LIB" ]; then
    print_error "$ASL_LIB not found."
    exit 1
fi
print_success "Core libraries present."

# Extract the core libraries to temporary directories.
print_info "Extracting core libraries..."
## ASL.
tar -xzf "$ASL_LIB" -C "$BUILD_DIR"
if [ ! -d "$BUILD_DIR/default_libasl/include/asl" ]; then
    print_error "include/asl/ directory not found (includes will not be available)."
    exit 1
fi
if [ ! -f "$BUILD_DIR/default_libasl/lib/libasl.a" ]; then
    print_error "lib/libasl.a not found."
    exit 1
fi
print_success "Core libraries extracted."

# Build the example.
print_info "Building LIFO basic example..."
print_info "Entering build directory..."
cd "$BUILD_DIR"
## Compile source files.
export CORE_INC_PATH="-I default_libasl/include/asl -I ./../"
print_info "Compiling main.c..."
gcc $CFLAGS $CORE_INC_PATH -c ../main.c
print_info "Compiling basic_lifo.c..."
gcc $CFLAGS $CORE_INC_PATH -c ../basic_lifo.c
# Link executable
print_info "Linking basic_lifo..."
gcc main.o basic_lifo.o -L ./default_libasl/lib -lasl -static -o basic_lifo_example
# Removing temporary build files.
print_info "Cleaning up temporary build files..."
rm -rf *.o
rm -rf *.a
rm -rf default_libasl
print_success "Build completed successfully!"

print_success "Exit script."
exit 0;
