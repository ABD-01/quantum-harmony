// 02_abi.c
// ABI = how functions pass arguments, return values, lay out structs in memory
// MSVC and GNU (MinGW) have differences here, especially in C++
// This file explores the C side of ABI

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// --- Struct layout ---
// Both compilers generally agree on basic C structs
// but diverge on edge cases and C++ classes

typedef struct {
    char   a;   // 1 byte
    int    b;   // 4 bytes - where does it start?
    char   c;   // 1 byte
    double d;   // 8 bytes - where does it start?
} PaddingTest;

typedef struct {
    uint8_t  x;
    uint64_t y;
    uint8_t  z;
} AlignTest;

// --- Long double ---
// THIS IS A KEY DIFFERENCE:
// MSVC:   long double == double (64-bit)
// MinGW:  long double == 80-bit extended precision (x87)
void test_long_double() {
    printf("\n--- long double size ---\n");
    printf("sizeof(float)       = %zu\n", sizeof(float));
    printf("sizeof(double)      = %zu\n", sizeof(double));
    printf("sizeof(long double) = %zu\n", sizeof(long double));

    // If you see 8, you're in MSVC-land
    // If you see 12 or 16, you're in GNU-land
}

// --- __int64 vs long long ---
// MSVC historically used __int64, GNU uses long long
// Both should work with modern compilers but let's check
void test_integer_types() {
    printf("\n--- integer type sizes ---\n");
    printf("sizeof(int)       = %zu\n", sizeof(int));
    printf("sizeof(long)      = %zu\n", sizeof(long));  // <-- THIS DIFFERS!
    printf("sizeof(long long) = %zu\n", sizeof(long long));
    printf("sizeof(void*)     = %zu\n", sizeof(void*));

    // KEY DIFFERENCE: sizeof(long) on Windows
    // MSVC / clang-cl (windows-msvc): long = 4 bytes (LLP64 model)
    // MinGW (windows-gnu):            long = 4 bytes (also LLP64 on Windows)
    // Linux GCC:                      long = 8 bytes (LP64 model)
    // So on Windows both agree, but if you're cross-compiling from Linux, surprise!
}

void test_struct_layout() {
    printf("\n--- struct layout ---\n");
    printf("sizeof(PaddingTest) = %zu\n", sizeof(PaddingTest));
    printf("  offsetof(a) = %zu\n", offsetof(PaddingTest, a));
    printf("  offsetof(b) = %zu\n", offsetof(PaddingTest, b));
    printf("  offsetof(c) = %zu\n", offsetof(PaddingTest, c));
    printf("  offsetof(d) = %zu\n", offsetof(PaddingTest, d));

    printf("sizeof(AlignTest) = %zu\n", sizeof(AlignTest));
    printf("  offsetof(x) = %zu\n", offsetof(AlignTest, x));
    printf("  offsetof(y) = %zu\n", offsetof(AlignTest, y));
    printf("  offsetof(z) = %zu\n", offsetof(AlignTest, z));
}

int main() {
    test_long_double();
    test_integer_types();
    test_struct_layout();
    return 0;
}
