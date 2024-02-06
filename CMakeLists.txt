cmake_minimum_required(VERSION 3.27.0)
project("MQTT Client using Paho" VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")

# Specify the path to Paho libraries and include directories
set(eclipse-paho-mqtt-c_DIR "C:/Users/Muhammed/Downloads/moquittoCerts/paho.mqtt.c/build/local/")
set(PAHO_LIB_DIR "${eclipse-paho-mqtt-c_DIR}/lib")
set(PAHO_INCLUDE_DIR "${eclipse-paho-mqtt-c_DIR}/include")
set(PAHO_BIN_DIR "${eclipse-paho-mqtt-c_DIR}/bin")

# Add your source files
set(SOURCE_FILES
    mqtt_client.c
    hexdump/hexdump.c
    hexdump/hexdump.h
)

# Add executable target
add_executable(mqtt_client_sub ${SOURCE_FILES})

# Include Paho headers
target_include_directories(mqtt_client_sub PRIVATE ${PAHO_INCLUDE_DIR})

# include hexdump header
target_include_directories(mqtt_client_sub PRIVATE hexdump)

# set library directory 
target_link_directories(mqtt_client_sub PRIVATE ${PAHO_LIB_DIR})

# Link against Paho libraries
target_link_libraries(mqtt_client_sub PRIVATE
    paho-mqtt3cs
    # paho-mqtt3as
    # Add other Paho libraries as needed
)


# Copy necessary DLLs to the output directory (for Windows)
if(WIN32)
    add_custom_command(TARGET mqtt_client_sub POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PAHO_BIN_DIR}/libpaho-mqtt3cs.dll"
            $<TARGET_FILE_DIR:mqtt_client_sub>)
endif()

message("Processing CMakeLists.txt")

message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")


# cmake .. -DCMAKE_C_COMPILER='C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/Llvm/bin/clang.exe'
# cmake .. -G "MSYS Makefiles" -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_AR=C:/msys64/ucrt64/bin/ar.exe