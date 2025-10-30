# CMake minimum version.
cmake_minimum_required(VERSION 3.14)

# Project definition.
project(asl_examples VERSION 0.0.1 LANGUAGES C)
message(STATUS "ATP_ROOT is ${ATP_ROOT}")
message(STATUS "TARGET_PLATFORM is ${TARGET_PLATFORM}")
message(STATUS "CMAKE_TOOLCHAIN_FILE is ${CMAKE_TOOLCHAIN_FILE}")

# Include tools.
include(FetchContent)

# Set absolute paths to tarballs.
set(REQUIRED_ASL_VERSION "0.0.1")
set(ASL_TARBALL_PATH "${ATP_ROOT}/libs/asl/libasl_${REQUIRED_ASL_VERSION}_${TARGET_PLATFORM}.tar.gz")

# Fetch ASL library.
FetchContent_Declare(
    lib_asl
    URL "file://${ASL_TARBALL_PATH}"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/extracted_asl"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

# Make libraries available.
FetchContent_MakeAvailable(lib_asl)

# Update CMAKE_PREFIX_PATH.
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_BINARY_DIR}/extracted_asl"
)

# Find required libraries.
find_package(asl ${REQUIRED_ASL_VERSION} EXACT REQUIRED)

# Add example subdirectories.
add_subdirectory(basic_cbuf)
add_subdirectory(basic_fifo)
add_subdirectory(basic_lifo)
