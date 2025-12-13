# CMakeLists.txt
# 
# Copyright (c) 2023-24 Accolade Electronics Pvt Ltd. All Rights Reserved.
#
# Date:     June 25, 2025
# Modified: June 25, 2025
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Brief:    This CMakeLists.txt file is used to configure the build system
#           for the project.

cmake_minimum_required(VERSION 3.27.0)
project(cens_arena
    VERSION     0.0.1
    LANGUAGES   C CXX
    DESCRIPTION "Linear Memory Allocator"
)


set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(DEFAULT_BUILD_TYPE "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Install Path
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})

option(ENABLE_TESTS "Enable testing" ON)
option(ENABLE_DEBUG "Enable debug features" OFF)  # ON = 1 by default

# Create Library
add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/arena.c
)


# Include Files:
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(${PROJECT_NAME}_PUBLIC_HEADERS
    arena.h
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        VERSION         ${PROJECT_VERSION}
        SOVERSION       ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER   "${${PROJECT_NAME}_PUBLIC_HEADERS}"
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE
    #    ENABLE_DEBUG=$<BOOL:${ENABLE_DEBUG}>
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# CRT runtime (for MSVC only)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    #  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

# tests
if(ENABLE_TESTS AND NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    enable_testing()
    #set(GTest_DIR "${CMAKE_SOURCE_DIR}/third_party/googletest/install/lib/cmake/GTest")
    find_package(GTest REQUIRED)
    message(STATUS "GoogleTest found: ${GTest_DIR}")

    add_executable(test_arena test_arena.cc)
    target_link_libraries(test_arena PRIVATE cens_arena GTest::gtest_main)
    add_test(NAME test_arena COMMAND test_arena)
endif() 

# install docs
# install(DIRECTORY docs/doxygen/html DESTINATION ${CMAKE_INSTALL_DOCDIR})

