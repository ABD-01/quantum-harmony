# CMakeLists.txt
# 
# Copyright (c) 2023-24 Accolade Electronics Pvt Ltd. All Rights Reserved.
#
# Date:     May 29, 2024
# Modified: June 5, 2024
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Brief:    This CMakeLists.txt file is used to configure the build system
#           for the project.

cmake_minimum_required(VERSION 3.27.0)
project(BumbleNet
    VERSION     0.0.1
    LANGUAGES   C
    DESCRIPTION "Network Library"
)

# ---- Include guards ----
#ref: https://github.com/ClausKlein/cmake-example-component-lib/blob/develop/CMakeLists.txt#L14
include(cmake/PreventInSourceBuilds.cmake)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Install Path
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})

# Option to enable testing
option(BumbleNet_ENABLE_TESTING "Enable testing" ON)
set(NUMCOLS 24 CACHE STRING "Number of columns in hexdump")

# Create Library
add_library(${PROJECT_NAME} STATIC)

# Source Files:
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hexdump.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.c
)
target_sources(${PROJECT_NAME} 
    PRIVATE 
        ${SOURCE_FILES}
)

# Include Files:
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(${PROJECT_NAME}_PUBLIC_HEADERS
        include/hexdump.h
        include/bn_logger.h
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        C_STANDARD      99 
        VERSION         ${PROJECT_VERSION}
        SOVERSION       ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER   "${${PROJECT_NAME}_PUBLIC_HEADERS}"
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE
        NUMCOLS=${NUMCOLS}
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_subdirectory(config_files)

if(BumbleNet_ENABLE_TESTING AND NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    enable_testing()
    # add_subdirectory(third_party/googletest)
    add_subdirectory(third_party/unity)
    add_subdirectory(test)
endif()

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

MESSAGE(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})