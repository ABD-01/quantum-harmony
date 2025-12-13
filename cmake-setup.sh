
cmake .. -DCMAKE_INSTALL_PREFIX=../install \
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
    -G "MinGW Makefiles" \
    -DCMAKE_MAKE_PROGRAM="C:/CSDTK4/make64/make.exe" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL \
    -DGTest_DIR=./third_party/googletest/install/lib/cmake/GTest
