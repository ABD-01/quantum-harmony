# POSIX example for HAL CBUF.
set(APP_NAME "basic_cbuf")

# Set sources.
set(APP_SOURCES
    basic_cbuf.c
    main.c
)

# Setup executable.
add_executable(${APP_NAME} ${APP_SOURCES})

# Setup dependencies.
target_link_libraries(${APP_NAME} PRIVATE
    asl::asl
)

# Setup include directories.
target_include_directories(${APP_NAME} PRIVATE .)

# Set linker options.
target_link_options(${APP_NAME} PRIVATE -pthread)
