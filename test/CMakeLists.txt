project(BumbleNet-tests)

# TEST1

add_executable(
    test1 test1.c
)

target_link_libraries(
    test1 BumbleNet
)

add_test(
    NAME test1
    COMMAND test1 125
)

# TEST2

add_executable(test2 test2.c)

target_link_libraries(test2 BumbleNet unity)

add_test(NAME test2 COMMAND test2)


# TEST3

set(GTest_DIR "${CMAKE_SOURCE_DIR}/third_party/googletest/install/lib/cmake/GTest")

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/googletest/install" ${CMAKE_PREFIX_PATH})

find_package(GTest REQUIRED)

message(STATUS "GoogleTest found: ${GTest_DIR} ${GTest_INCLUDE_DIRS} ${GTest_LIBRARIES}")

add_executable(
    test3 test3.cc
)

target_link_libraries(test3 BumbleNet GTest::gtest GTest::gtest_main)

add_test(
    NAME test3
    COMMAND test3
)
