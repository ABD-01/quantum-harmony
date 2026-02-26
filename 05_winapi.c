// 05_winapi.c
// Both compilers target Windows so WinAPI should work on both
// but there are subtle runtime differences

#include <stdio.h>
#include <windows.h>

// --- Which C runtime are we linked to? ---
// MSVC clang-cl  -> links to vcruntime140.dll + ucrt (Universal CRT)
// llvm-mingw msvcrt -> links to msvcrt.dll  (old, ships with Windows)
// llvm-mingw ucrt   -> links to ucrt (newer)
//
// This matters because:
//   - You cannot mix FILE* handles across runtime boundaries
//   - malloc() in one runtime, free() in another = heap corruption
//   - The runtimes have different behaviors for edge cases

void test_runtime_identity() {
    printf("\n--- runtime identity ---\n");

#if defined(_UCRT)
    printf("C Runtime: Universal CRT (ucrt)\n");
#elif defined(__MSVCRT_VERSION__)
    printf("C Runtime: msvcrt (version 0x%X)\n", __MSVCRT_VERSION__);
#else
    printf("C Runtime: msvcrt (classic)\n");
#endif

    // Check if we're using secure functions
#if defined(__STDC_SECURE_LIB__)
    printf("Secure lib: yes (__STDC_SECURE_LIB__ = %d)\n", __STDC_SECURE_LIB__);
#else
    printf("Secure lib: no\n");
#endif
}

// --- printf format strings ---
// Classic msvcrt.dll does NOT support %zu, %lld properly
// ucrt and modern msvcrt do
// This caused a LOT of pain for MinGW users historically

void test_printf_formats() {
    printf("\n--- printf format string support ---\n");
    
    size_t    sz  = 12345;
    long long ll  = -9876543210LL;
    
    printf("%%zu  -> %zu\n",  sz);   // should print 12345
    printf("%%lld -> %lld\n", ll);   // should print -9876543210
    printf("%%p   -> %p\n",   (void*)&sz);
}

// --- Windows API basics ---
void test_winapi() {
    printf("\n--- WinAPI ---\n");
    
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    printf("Processor architecture: %u\n", si.wProcessorArchitecture);
    printf("  (9=x64, 12=ARM64, 0=x86)\n");
    printf("Number of processors:   %lu\n", si.dwNumberOfProcessors);
    printf("Page size:              %lu bytes\n", si.dwPageSize);

    MEMORYSTATUSEX ms = { sizeof(ms) };
    GlobalMemoryStatusEx(&ms);
    printf("Physical RAM: %llu MB\n", ms.ullTotalPhys / (1024*1024));
}

int main() {
    test_runtime_identity();
    test_printf_formats();
    test_winapi();
    return 0;
}
