cmake_minimum_required(VERSION 3.27.0)
project("MQTT Client using Paho" VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")

# Specify the path to Paho libraries and include directories
set(eclipse-paho-mqtt-c_DIR "C:/Users/Muhammed/Downloads/moquittoCerts/paho.mqtt.c/build/local/")
set(PAHO_LIB_DIR "${eclipse-paho-mqtt-c_DIR}/lib")
set(PAHO_INCLUDE_DIR "${eclipse-paho-mqtt-c_DIR}/include")
set(PAHO_BIN_DIR "${eclipse-paho-mqtt-c_DIR}/bin")

# Define NUMCOLS during the build (default value is 24)
set(NUMCOLS 24 CACHE INTEGER "Number of columns in hexdump")

# Add your source files
set(SOURCE_FILES
    mqtt_client.c
    hexdump/hexdump.c
)

# Function to add an executable target and copy DLL
function(add_mqtt_target target_name ssl_enabled)
    add_executable(${target_name} ${SOURCE_FILES})
    
    # Include Paho headers
    target_include_directories(${target_name} PRIVATE ${PAHO_INCLUDE_DIR})
    
    # Include hexdump header
    target_include_directories(${target_name} PRIVATE hexdump)

    # Set library directory
    target_link_directories(${target_name} PRIVATE ${PAHO_LIB_DIR})

    # Pass NUMCOLS as preprocessor definition
    target_compile_definitions(${target_name} PRIVATE NUMCOLS=${NUMCOLS})
    
    # Link against Paho libraries based on SSL
    if (${ssl_enabled})
        target_link_libraries(${target_name} PRIVATE paho-mqtt3cs)
        target_compile_definitions(${target_name} PRIVATE SSL_ENABLED)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PAHO_BIN_DIR}/libpaho-mqtt3cs.dll"
                $<TARGET_FILE_DIR:${target_name}>)
    else()
        target_link_libraries(${target_name} PRIVATE paho-mqtt3c)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PAHO_BIN_DIR}/libpaho-mqtt3c.dll"
                $<TARGET_FILE_DIR:${target_name}>)
    endif()
endfunction()

# Build with SSL support
add_mqtt_target(mqtt_client_ssl TRUE)

# Build without SSL support
add_mqtt_target(mqtt_client FALSE)

message("Processing CMakeLists.txt")
message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")


# cmake .. -DCMAKE_C_COMPILER='C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/Llvm/bin/clang.exe'
# cmake .. -G "MSYS Makefiles" -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_AR=C:/msys64/ucrt64/bin/ar.exe