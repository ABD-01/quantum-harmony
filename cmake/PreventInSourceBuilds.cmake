# cmake/PreventInSourceBuilds.cmake
#
# Copyright (c) 2024 Accolade Electronics Pvt. Ltd.
# 
# Date:       June 5, 2024
# Author:     Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# 
# Brief:      This function will prevent in-source builds
# Reference:  https://github.com/ClausKlein/cmake_template/blob/develop/cmake/PreventInSourceBuilds.cmake

function(myproject_assure_out_of_source_builds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if("${srcdir}" STREQUAL "${bindir}")
    message("######################################################")
    message("Warning: in-source builds are disabled")
    message("Please create a separate build directory and run cmake from there")
    message("######################################################")
    message(FATAL_ERROR "Quitting configuration")
  endif()
endfunction()

myproject_assure_out_of_source_builds()
