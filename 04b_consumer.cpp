// 04b_consumer.cpp
// Compile this AFTER 04a_lib.cpp, with the OPPOSITE compiler
// Then try to link and watch what happens

#include <stdio.h>
#include <string>

// Declare the C exports - these should link fine
extern "C" {
    __declspec(dllimport) int add_numbers(int a, int b);
    __declspec(dllimport) void print_message(const char* msg);
}

// Declare the C++ exports - these will cause linker errors
// if compiled with a different ABI
__declspec(dllimport) std::string get_greeting();

int main() {
    printf("=== Cross-ABI DLL experiment ===\n\n");

    // This should work even across ABI boundaries (C linkage)
    printf("Testing C exports:\n");
    int result = add_numbers(3, 4);
    printf("  add_numbers(3, 4) = %d\n", result);
    print_message("hello from consumer");

    // This will likely crash or fail to link across ABI boundaries
    printf("\nTesting C++ exports (danger zone):\n");
    std::string greeting = get_greeting();
    printf("  greeting = %s\n", greeting.c_str());

    return 0;
}
