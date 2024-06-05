# config_files/CMakeLists.txt
#
# Copyright (c) 2024 Accolade Electronics Pvt. Ltd.
# 
# Date:     June 5, 2024
# Author:   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
# Breif:    A very simple example of a configured file that might need to be
#           converted to one that is publicly installed in the case that
#           you are developing a library
# Ref:      https://github.com/ClausKlein/cmake_template/blob/develop/configured_files/CMakeLists.txt

configure_file("config.h.in" "${CMAKE_BINARY_DIR}/config_files/include/config.h" ESCAPE_QUOTES)