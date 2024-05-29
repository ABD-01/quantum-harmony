project(BumbleNet-tests)

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


add_executable(
    test2 test2.c
)

target_link_libraries(
    test2 BumbleNet unity
)

add_test(
    NAME test2
    COMMAND test2
)

add_executable(
    test3 test3.cc
)

target_link_libraries(
    test3 BumbleNet gtest gmock
)

add_test(
    NAME test3
    COMMAND test3
)
