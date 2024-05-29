cmake_minimum_required(VERSION 3.27.0)
project(BumbleNet
    VERSION     0.0.1
    LANGUAGES   C
    DESCRIPTION "Network Library"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Install Path
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/armgcc.cmake)

# Option to enable testing
option(BumbleNet_ENABLE_TESTING "Enable testing" ON)
set(NUMCOLS 24 CACHE STRING "Number of columns in hexdump")

# Create Library
add_library(${PROJECT_NAME} STATIC)

# Source Files:
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hexdump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.c
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
        PUBLIC_HEADER   "${${PROJECT_NAME}_PUBLIC_HEADERS}"
)

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE
        NUMCOLS=${NUMCOLS}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BumbleNet_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(third_party/googletest)
    add_subdirectory(third_party/unity)
    add_subdirectory(test)
endif()

MESSAGE(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})