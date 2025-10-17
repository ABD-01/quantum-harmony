# CMake minimum version.
cmake_minimum_required(VERSION 3.14)

# Project definition.
project(asl VERSION 0.0.1 LANGUAGES C)
message(STATUS "ATP_ROOT is ${ATP_ROOT}")
message(STATUS "TARGET_PLATFORM is ${TARGET_PLATFORM}")
message(STATUS "CMAKE_TOOLCHAIN_FILE is ${CMAKE_TOOLCHAIN_FILE}")

# Derive a reusable module name from the project name.
set(MOD_NAME "${PROJECT_NAME}")
# Namespace used for export.
set(MOD_NAMESPACE "${MOD_NAME}::")

# Include tools.
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "${MOD_NAME}")
set(CPACK_PACKAGE_FILE_NAME "${TARGET_PLATFORM}_lib${MOD_NAME}")
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CPack)

# Define source files for this module.
set(MOD_SOURCES
    # CBUF.
    asl/library/asl_cbuf.c
    # FIFO.
    asl/library/asl_fifo.c
    # LIFO.
    asl/library/asl_lifo.c
    # Utilities.
    asl/library/asl_util.c
)

# Define header files that will be installed (exported).
set(MOD_PUBLIC_HEADERS
    # CBUF.
    asl/library/asl_cbuf.h
    asl/library/asl_cbuf_types.h
    # FIFO.
    asl/library/asl_fifo.h
    asl/library/asl_fifo_types.h
    # LIFO.
    asl/library/asl_lifo.h
    asl/library/asl_lifo_types.h
    # Utilities.
    asl/library/asl_util.h
    asl/library/asl_util_types.h
    # Foundational types.
    asl/defs/asl_iface_composite_priv_types.h
    asl/defs/asl_iface_composite_types.h
    asl/defs/asl_iface_priv_types.h
    asl/defs/asl_iface_types.h
    asl/defs/asl_pointer_types.h
    asl/defs/asl_types.h
    # Standard types that are missing in some toolchains.
    asl/std/inttypes.h
)

# Specify static library.
add_library(${MOD_NAME} STATIC ${MOD_SOURCES})

# Set build interfaces and install (export) interfaces.
target_include_directories(${MOD_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/asl/library>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/asl/defs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/asl/std>
    $<INSTALL_INTERFACE:include/${MOD_NAME}>
)

# Install archive using canonical layout.
install(TARGETS ${MOD_NAME}
    EXPORT ${MOD_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers using canonical layout.
install(FILES ${MOD_PUBLIC_HEADERS}
    DESTINATION include/${MOD_NAME}
)

# Create a small temporary config template in the build tree.
set(_mod_config_template "${CMAKE_CURRENT_BINARY_DIR}/${MOD_NAME}Config.cmake.in")
file(WRITE "${_mod_config_template}"
    "@PACKAGE_INIT@\n"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}Targets.cmake\")\n"
)

# Configure the package config file from the generated template, install into proper cmake dir.
configure_package_config_file(
    "${_mod_config_template}"
    "${CMAKE_CURRENT_BINARY_DIR}/${MOD_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${MOD_NAME}"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${MOD_NAME}Config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${MOD_NAME}"
)

# Install exported targets into a cmake directory inside the package.
install(EXPORT ${MOD_NAME}Targets
    FILE ${MOD_NAME}Targets.cmake
    NAMESPACE ${MOD_NAMESPACE}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${MOD_NAME}
)

# Write a package version config file, install into proper cmake dir.
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${MOD_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${MOD_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${MOD_NAME}
)

# Custom target to package and publish
add_custom_target(package_and_publish
    COMMAND ${CMAKE_COMMAND} --build . --target package
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ATP_ROOT}/libs/${MOD_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${TARGET_PLATFORM}_lib${MOD_NAME}.tar.gz ${ATP_ROOT}/libs/${MOD_NAME}/
)
