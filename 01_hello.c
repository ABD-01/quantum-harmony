// 01_hello.c
// Most basic test - both compilers should handle this fine
// If this fails, your compiler setup is broken

#include <stdio.h>

int main() {
    printf("Hello from: " __VERSION__ "\n");
    printf("Compiled on: " __DATE__ " " __TIME__ "\n");

#if defined(_MSC_VER)
    printf("Compiler persona: MSVC (cl.exe style)\n");
    printf("_MSC_VER = %d\n", _MSC_VER);
#elif defined(__MINGW64__)
    printf("Compiler persona: MinGW-w64\n");
#elif defined(__MINGW32__)
    printf("Compiler persona: MinGW-32\n");
#endif

#if defined(__clang__)
    printf("Frontend: Clang %d.%d.%d\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif

#if defined(_WIN32)
    printf("Target OS: Windows\n");
#endif

    return 0;
}
