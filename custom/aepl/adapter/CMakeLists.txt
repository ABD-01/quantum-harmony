# custom/aepl/adapter/CMakeLists.txt
#
# Copyright (c) 2025 Accolade Electronics Pvt. Ltd.
# 
# Date:     April 4, 2025
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# 
# Brief:    Build script for the adapter static library

add_library(adapter STATIC)

target_sources(adapter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mcu_bridge.c
)

target_include_directories(adapter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(adapter_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter_struct.h
)

set_target_properties(adapter
    PROPERTIES
    PUBLIC_HEADER "${adapter_PUBLIC_HEADERS}"
)

target_compile_options(adapter PRIVATE
    -EL

    -Wall
    -Wextra
)