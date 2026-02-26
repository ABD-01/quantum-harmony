// 04_dll_boundary/
// This experiment requires two files: a "library" and a "consumer"
// The point: compile the library with one compiler, consumer with another
// Watch the linker scream

// === 04a_lib.cpp === (compile this to a .dll or .a first)
// This is the "library" side

#ifdef BUILD_LIB

#include <stdio.h>
#include <string>

// Pure C export - this WILL work across ABI boundaries
// because C has no mangling
extern "C" {
    __declspec(dllexport) int add_numbers(int a, int b) {
        return a + b;
    }

    __declspec(dllexport) void print_message(const char* msg) {
        printf("Library says: %s\n", msg);
    }
}

// C++ export - this will BREAK across ABI boundaries
// No extern "C" = mangled name = ABI-specific
__declspec(dllexport) std::string get_greeting() {
    return "Hello from the library!";
}

// C++ class export - extremely fragile across ABIs
class __declspec(dllexport) MyLibClass {
public:
    MyLibClass() { printf("MyLibClass constructed\n"); }
    ~MyLibClass() { printf("MyLibClass destroyed\n"); }
    virtual void doSomething() { printf("MyLibClass::doSomething()\n"); }
    int value = 42;
};

#endif // BUILD_LIB
