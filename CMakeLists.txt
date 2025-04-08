# CMakeLists.txt
# 
# Copyright (c) 2023-24 Accolade Electronics Pvt Ltd. All Rights Reserved.
#
# Created:  April 3, 2025
# Modified: April 8, 2025
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Brief:    This CMakeLists.txt file is used to configure the build system
#           for the project.

cmake_minimum_required(VERSION 3.27.0)
project(MC25_telematics
    VERSION     1.0.0 
    LANGUAGES   C ASM
    DESCRIPTION "MC25 Telematics"
)

set(PLATFORM "MC25_telematics")
set(MEMORY "M32")
set(VERSION "A01")
set(TARGET "${PLATFORM}${MEMORY}${VERSION}")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(OBJECT_DIR ${CMAKE_BINARY_DIR}/obj)

add_subdirectory(custom/aepl/adapter)

add_executable(${TARGET})

target_sources(${TARGET} PRIVATE
    # custom sources
    custom/command_handler.c
    custom/command_processor.c
    custom/console.c
    custom/debug.c
    custom/gnss.c
    custom/http_test.c
    custom/json_helpers.c
    custom/main.c
    custom/mc25_dfota.c
    custom/mcu_comms.c
    custom/mqtt.c
    custom/network.c
    custom/nvm_storage.c
    custom/protobuf_messaging.c
    custom/remote_commands.c
    custom/settings.c
    custom/spi_flash_memory.c
    custom/telemetry.c
    custom/telemetry_history.c
    custom/time.c
    custom/util.c
    custom/vehicle.c

    # System Config
    custom/config/custom_sys_cfg.c
    custom/config/sys_config.c

    # DFota Sources
    custom/dfota/src/dfota_ftp.c
    custom/dfota/src/dfota_http.c
    custom/dfota/src/dfota_http_code.c
    custom/dfota/src/dfota_main.c

    # Demo Sources
    custom/aepl/adapter/demo.c
)

# System Configuration
# file(GLOB SRC_SYS "custom/config/*.c")
# target_sources(${TARGET} PRIVATE ${SRC_SYS})

# RIL sources
file(GLOB SRC_SYS_RIL "ril/src/*.c")
target_sources(${TARGET} PRIVATE ${SRC_SYS_RIL})

# Custom sources (root of custom directory)
# file(GLOB SRC_CUS "custom/*.c")
# target_sources(${TARGET} PRIVATE ${SRC_CUS})

# DFOTA sources
# file(GLOB SRC_DFOTA "custom/dfota/src/*.c")
# target_sources(${TARGET} PRIVATE ${SRC_DFOTA})

# Protobuf sources
file(GLOB SRC_PROTOBUF "custom/protobuf/*.c")
target_sources(${TARGET} PRIVATE ${SRC_PROTOBUF})

# demo sources
# file(GLOB SRC_DEMO "custom/aepl/adapter/demo*.c")
# target_sources(${TARGET} PRIVATE ${SRC_DEMO})

# Define all include directories (from INCS in the original Makefile)
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}              # Root dir
    ${CMAKE_SOURCE_DIR}/include      # SDK includes
    ${CMAKE_SOURCE_DIR}/ril/inc      # RIL includes
    ${CMAKE_SOURCE_DIR}/custom/config    # Custom config
    ${CMAKE_SOURCE_DIR}/custom/dfota/inc # DFOTA includes
    ${CMAKE_SOURCE_DIR}/custom/protobuf  # Protobuf includes
    ${CMAKE_SOURCE_DIR}/custom/aepl/adapter # Adapter includes
    ${CMAKE_SOURCE_DIR}/custom        # Custom includes
)

target_compile_definitions(${TARGET} PRIVATE
    __CUSTOMER_CODE__
    PB_NO_STATIC_ASSERT
    __OCPU_COMPILER_GCC__
    __have_long64
    __int_fast64_t_defined
)

target_compile_options(${TARGET} PRIVATE
    -Wshadow
    -Wundef
    -Wunused-function
    -Wall
    -minterlink-mips16
    -fno-inline-small-functions
    -fno-inline-functions
    -fno-align-functions
    -fno-align-jumps
    -fno-align-loops
    -fno-align-labels
    -pipe
    -fwide-exec-charset=UTF-16LE
    -fshort-wchar
    -fno-strict-aliasing
    -ffunction-sections
    -fdata-sections
    -fno-builtin-iswspace
    -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7
    -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7
    -ffixed-fp
    -G0
    -mexplicit-relocs
    -fweb
    -frename-registers
    -mmemcpy
    -mmips-tfile
    -nostartfiles

    $<$<COMPILE_LANGUAGE:CXX>:
    -Wno-volatile
    -Wold-style-cast
    -Wuseless-cast
    -Wsuggest-override>
    $<$<CONFIG:Debug>:-O0 -g3 -ggdb -Wextra>
    $<$<CONFIG:Release>:-Os -g0>
)

target_link_libraries(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/gcc/fplibm.lib
    ${CMAKE_SOURCE_DIR}/libs/gcc/app_start.lib
    adapter
)

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "MIPS")
    target_link_libraries(${TARGET} PRIVATE
        ${MIPS_TOOLCHAIN_PATH}/lib/gcc/mips-elf/${GCC_VERSION}/libgcc.a
    )
ENDIF()

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/libs/gcc/linkscript.ld")
target_link_options(${TARGET} PRIVATE
    -T ${LINKER_SCRIPT}
    --oformat=elf32-littlemips
    -Wl,-Rbuild
    -nodefaultlibs
    --warn-common
    --error-unresolved-symbols
    -Wl,-Map=${CMAKE_BINARY_DIR}/bin/${TARGET}.map
)
# Override the linker command entirely to use mips-elf-ld directly
# set(CMAKE_C_LINK_EXECUTABLE
#     "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
# )
# set(CMAKE_EXE_LINKER_FLAGS "--oformat=elf32-littlemips -T${LINKER_SCRIPT} -Rbuild -nodefaultlibs --warn-common --error-unresolved-symbols -Map=${CMAKE_BINARY_DIR}/bin/${TARGET}.map")


add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET}>)

# Generate binary output
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${CMAKE_BINARY_DIR}/bin/${TARGET}.bin
    COMMENT "Generating binary file ${TARGET}.bin"
)

# Generate lod file (assuming you have the tool)
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/tools/gen_lod ${CMAKE_BINARY_DIR}/bin/${TARGET}.bin 137428992 4096 ${CMAKE_BINARY_DIR}/bin/${TARGET}.lod
    COMMENT "Generating lod file ${TARGET}.lod"
)

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS adapter
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

MESSAGE(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})