# A very simple example of a configured file that might need to be
# converted to one that is publicly installed in the case that
# you are developing a library
configure_file("config.h.in" "${CMAKE_BINARY_DIR}/config_files/include/config.h" ESCAPE_QUOTES)