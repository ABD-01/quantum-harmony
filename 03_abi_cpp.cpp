// 03_abi_cpp.cpp
// C++ ABI differences - this is where things get spicy
// Name mangling: how the compiler encodes function names in object files
//
// MSVC ABI:  ?foo@@YAXXZ  (Microsoft mangling)
// GNU ABI:   _Z3foov      (Itanium mangling, used by GCC/Clang on Linux and MinGW)
//
// If you compile a .lib with MSVC and try to link it with MinGW clang, you get
// "undefined reference" errors because the mangled names don't match

#include <stdio.h>

// --- Name mangling demo ---
// Compile this, then run:
//   nm 03_abi_cpp.o      (MinGW/Linux)
//   dumpbin /symbols 03_abi_cpp.obj  (MSVC)
// You'll see the mangled names directly

void plain_function() {}

void overloaded(int x)    { (void)x; }
void overloaded(double x) { (void)x; }
void overloaded(int x, double y) { (void)x; (void)y; }

namespace MyNS {
    void namespaced_fn() {}
    
    struct MyClass {
        void method() {}
        static void static_method() {}
        virtual void virtual_method() {}
    };
}

// --- Exception handling ABI ---
// MSVC uses SEH (Structured Exception Handling)
// MinGW uses SJLJ or DWARF or SEH depending on build
// Mixing these = crashes at exception boundaries

void test_exceptions() {
    printf("\n--- exception handling ---\n");
    try {
        throw 42;
    } catch (int e) {
        printf("Caught int: %d\n", e);
    }

    try {
        throw "a string";
    } catch (const char* e) {
        printf("Caught string: %s\n", e);
    }
}

// --- vtable layout ---
// Virtual function tables are laid out differently between MSVC and GNU ABI
// This is why you CANNOT pass a C++ object with virtual functions across
// a DLL boundary if the DLL was compiled with a different ABI

class Base {
public:
    virtual void whoami() { printf("I am Base\n"); }
    virtual ~Base() {}
};

class Derived : public Base {
public:
    void whoami() override { printf("I am Derived\n"); }
};

void test_vtable() {
    printf("\n--- vtable / virtual dispatch ---\n");
    Base* obj = new Derived();
    obj->whoami();  // should print Derived
    printf("sizeof(Base)    = %zu\n", sizeof(Base));
    printf("sizeof(Derived) = %zu\n", sizeof(Derived));
    delete obj;
}

int main() {
    test_exceptions();
    test_vtable();

    printf("\n--- check mangled names ---\n");
    printf("Compile this file, then inspect the .o/.obj:\n");
    printf("  MinGW:  nm 03_abi_cpp.o | grep overloaded\n");
    printf("  MSVC:   dumpbin /symbols 03_abi_cpp.obj | findstr overloaded\n");
    printf("You will see completely different name encodings\n");

    return 0;
}
