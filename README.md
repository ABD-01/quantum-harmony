# compiler-shenanigans

Hands-on experiments to feel the difference between:
- **Clang (MSVC ABI)** — the one inside Visual Studio BuildTools
- **Clang (GNU ABI / llvm-mingw)** — your project toolchain

## Setup: define your compilers

Open a terminal and set these once per session:

```console
$ source ~/.dotfiles/bash/setup_msvc_x64_env.sh
$ clang --version
clang version 19.1.5
Target: x86_64-pc-windows-msvc
Thread model: posix
InstalledDir: C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\Llvm\x64\bin
```

```console
$ source ~/.dotfiles/bash/mingw-clang.path
$ clang --version
clang version 16.0.6 (https://github.com/llvm/llvm-project.git 7cbf1a2591520c2491aa35339f227775f4d3adf6)
Target: x86_64-w64-windows-gnu
Thread model: posix
InstalledDir: D:/PROJECTS/../toolchains/llvm-mingw-20230614-msvcrt-x86_64/bin
```

## 01_hello.c — Identify yourself

Asks the compiler to reveal its identity via preprocessor macros.

```console
$ clang 01_hello.c -o hello_msvc.exe && ./hello_msvc.exe
Hello from: Clang 19.1.5
Compiled on: Feb 26 2026 14:29:04
Compiler persona: MSVC (cl.exe style)
_MSC_VER = 1944
Frontend: Clang 19.1.5
Target OS: Windows
```

```console
$ clang 01_hello.c -o hello_mingw.exe && ./hello_mingw.exe
Hello from: Clang 16.0.6 (https://github.com/llvm/llvm-project.git 7cbf1a2591520c2491aa35339f227775f4d3adf6)
Compiled on: Feb 26 2026 14:30:30
Compiler persona: MinGW-w64
Frontend: Clang 16.0.6
Target OS: Windows
```

**What to look for:** different `__VERSION__` strings and different `#define` macros
being triggered (`_MSC_VER` vs `__MINGW64__`).


## 02_abi.c — Size and layout differences

The most important one: `sizeof(long double)` will differ.

```console
$ clang 02_abi.c -o abi_msvc.exe && ./abi_msvc.exe

--- long double size ---
sizeof(float)       = 4
sizeof(double)      = 8
sizeof(long double) = 8

--- integer type sizes ---
sizeof(int)       = 4
sizeof(long)      = 4
sizeof(long long) = 8
sizeof(void*)     = 8

--- struct layout ---
sizeof(PaddingTest) = 24
  offsetof(a) = 0
  offsetof(b) = 4
  offsetof(c) = 8
  offsetof(d) = 16
sizeof(AlignTest) = 24
  offsetof(x) = 0
  offsetof(y) = 8
  offsetof(z) = 16
```

```console
$ clang 02_abi.c -o abi_mingw.exe && ./abi_mingw.exe

--- long double size ---
sizeof(float)       = 4
sizeof(double)      = 8
sizeof(long double) = 16

--- integer type sizes ---
sizeof(int)       = 4
sizeof(long)      = 4
sizeof(long long) = 8
sizeof(void*)     = 8

--- struct layout ---
sizeof(PaddingTest) = 24
  offsetof(a) = 0
  offsetof(b) = 4
  offsetof(c) = 8
  offsetof(d) = 16
sizeof(AlignTest) = 24
  offsetof(x) = 0
  offsetof(y) = 8
  offsetof(z) = 16
```

**What to look for:**
- `sizeof(long double)` → MSVC gives `8`, MinGW gives `12` or `16`
- Struct offsets are usually the same for basic C structs

## 03_abi_cpp.cpp — Name mangling

Compile to object file only (`-c`), then inspect the symbols.

Compile to .o only, don't link

```console
$ clang 03_abi_cpp.cpp -c -o 03_abi_mingw.o
$ nm 03_abi_mingw.o | rg overload
00000020 T _Z10overloadedd
00000010 T _Z10overloadedi
00000030 T _Z10overloadedid
```

```console
$ clang 03_abi_cpp.cpp -c -o 03_abi_msvc.obj
$ dumpbin //symbols 03_abi_msvc.obj | rg overload
0C8 00000010 SECT1  notype ()    External     | ?overloaded@@YAXH@Z (void __cdecl overloaded(int))
0C9 00000020 SECT1  notype ()    External     | ?overloaded@@YAXN@Z (void __cdecl overloaded(double))
0CA 00000030 SECT1  notype ()    External     | ?overloaded@@YAXHN@Z (void __cdecl overloaded(int,double))
```

**What to look for:**
- MinGW: `_Z10overloadedi`, `_Z10overloadedd` (Itanium mangling)
- MSVC:  `?overloaded@@YAXH@Z`, `?overloaded@@YAXN@Z` (Microsoft mangling)

These names are completely incompatible. This is why you can't link a
`.lib` built with MSVC into a MinGW project for C++ symbols.


## 04a + 04b — The DLL boundary disaster

This is the most educational experiment. Compile the library with one
compiler, the consumer with the other.

Step 1: Build the library with MSVC clang

```console
$ clang -DBUILD_LIB -shared 04a_lib.cpp -o lib_msvc.dll -Xlinker /IMPLIB:lib_msvc.lib
```

Step 2: Try to link consumer with MinGW clang

This will FAIL for the C++ function (get_greeting) but PASS for C functions

```console
$ clang 04b_consumer.cpp -o consumer.exe -L. -l:lib_msvc.lib
ld.lld: error: undefined symbol: __declspec(dllimport) get_greeting()
>>> referenced by 04b_consumer-c46b1b.o:(main)

ld.lld: error: undefined symbol: std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>::~basic_string()
>>> referenced by 04b_consumer-c46b1b.o:(main)
>>> referenced by 04b_consumer-c46b1b.o:(main)

ld.lld: error: undefined symbol: __gxx_personality_seh0
>>> referenced by 04b_consumer-c46b1b.o:(.xdata)
clang-16: error: linker command failed with exit code 1 (use -v to see invocation)
```

**Expected errors from MinGW linker:**
```
undefined reference to `get_greeting()'
```
But `add_numbers` and `print_message` will link fine because they're `extern "C"`.

**The lesson:** `extern "C"` is your only safe cross-ABI interface.
C++ across DLL boundaries with mixed compilers = pain.

### and vice versa

Step 1: Build the library with MinGW clang

```console
$ clang++ -DBUILD_LIB -shared 04a_lib.cpp -o lib_mingw.dll -Wl,--out-implib,lib_mingw.lib
```

Step 2: Try to link consumer with MSVC clang
```console
$ clang 04b_consumer.cpp -o consumer.exe -L. -llib_mingw
04b_consumer-cc8924.o : error LNK2019: unresolved external symbol "__declspec(dllimport) class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > __cdecl get_greeting(void)" (__imp_?get_greeting@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ) referenced in function main
consumer.exe : fatal error LNK1120: 1 unresolved externals
clang: error: linker command failed with exit code 1120 (use -v to see invocation)
```

## 05_winapi.c — Runtime identity

```console
$ clang 05_winapi.c -o winapi_msvc.exe  -lkernel32 && ./winapi_msvc.exe

--- runtime identity ---
C Runtime: Universal CRT (ucrt)
Secure lib: yes (__STDC_SECURE_LIB__ = 200411)

--- printf format string support ---
%zu  -> 12345
%lld -> -9876543210
%p   -> 00000052CB8FFCC0

--- WinAPI ---
Processor architecture: 9
  (9=x64, 12=ARM64, 0=x86)
Number of processors:   8
Page size:              4096 bytes
Physical RAM: 16157 MB
```

```console
$ clang 05_winapi.c -o winapi_mingw.exe -lkernel32 && ./winapi_mingw.exe

--- runtime identity ---
C Runtime: msvcrt (version 0x700)
Secure lib: yes (__STDC_SECURE_LIB__ = 200411)

--- printf format string support ---
%zu  -> 12345
%lld -> -9876543210
%p   -> 000000de4daff890

--- WinAPI ---
Processor architecture: 9
  (9=x64, 12=ARM64, 0=x86)
Number of processors:   8
Page size:              4096 bytes
Physical RAM: 16157 MB
```

**What to look for:**
- `C Runtime: ucrt` vs `C Runtime: msvcrt`
- `%zu` and `%lld` format strings (should work on both modern compilers, but
  historically MinGW with old msvcrt.dll would print garbage here)


## Quick reference: what the target triple means

```
x86_64  -  pc       -  windows  -  msvc
x86_64  -  w64      -  windows  -  gnu
  ^           ^            ^          ^
 arch       vendor         OS        ABI

vendor "pc"  = generic
vendor "w64" = MinGW-w64 project

ABI "msvc" = Microsoft ABI (MSVC name mangling, SEH exceptions, LLP64)
ABI "gnu"  = GNU/Itanium ABI (GCC-compatible mangling, DWARF/SJLJ exceptions)
```

## Key takeaways

| Topic | MSVC Clang | MinGW Clang |
|---|---|---|
| `long double` | 8 bytes (= double) | 12 or 16 bytes (x87 80-bit) |
| Name mangling | Microsoft (`?foo@@...`) | Itanium (`_Z3foo...`) |
| Exception model | SEH | DWARF/SJLJ/SEH (varies) |
| C++ across DLL | Works with MSVC DLLs | Only via `extern "C"` |
| Needs VS runtime | Yes (vcruntime140.dll) | No (msvcrt.dll or ucrt) |
| Cross-compile from Linux | No | Yes |
