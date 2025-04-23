# Building the Project

## Prerequisites

- CSDTK4.2 toolkit is required
- CSDTK4 can be unzipped to any location (C:\CSDTK4 is used as the default path)
- Ensure that `mips-elf-gcc` is located at `C:\CSDTK4\mips-elf-4.4.2\bin`
- (Optional) Enusre that `make.exe` is located at `C:\CSDTK4\make`.

### Basic Build Instructions

1. **Create and Enter Build Directory**:
   ```
   mkdir build && cd build
   ```

2. **Configure the Project**:
   ```
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/mips-toolchain.cmake -DCMAKE_MAKE_PROGRAM=C:/CSDTK4/make/make.exe -DCMAKE_BUILD_TYPE=Release ..
   ```
   - `-DCMAKE_TOOLCHAIN_FILE`: Specifies the MIPS toolchain configuration.
   - `-DCMAKE_BUILD_TYPE=Release`: Builds with optimizations enabled (use `Debug` for debugging).
   - `-DCMAKE_MAKE_PROGRAM`: Points to the `make` executable from CSDTK4 (optional)

3. **Build the Project**:
   ```
   cmake --build .
   ```
   - Alternatively, since "Unix Makefiles" is the generator, you can run:
     ```
     make
     ```
   - This builds the `MC25_telematicsM32A01.elf`, `.bin`, and `.lod` files in `build/bin/`.

### Building Specific Targets

- **Build Only the Adapter Library**:
  ```
  cmake --build . --target adapter
  ```
  - Equivalent to `make adapter`.
  - Builds the `adapter` static library (`libadapter.a`) in `build/lib/`.

### Cleaning the Build

- **Clean the Build Directory**:
  ```
  cmake --build . --target clean
  ```
  - Equivalent to `make clean`.
  - Removes all generated build artifacts.

### Toolchain Path Configuration

If the CSDTK4 toolkit is installed at a location other than `C:\CSDTK4`, adjust the toolchain path using one of these methods:

1. **Using an Environment Variable**:
Set the `MIPSGCC_DIR` environment variable before running `cmake`:

```
export MIPSGCC_DIR="C:\path\to\mips-elf-4.4.2"
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/mips-toolchain.cmake ..
```

2. **Update the Toolchain File**:

Edit `cmake/mips-toolchain.cmake` and update the default path:
```cmake
    SET(MIPS_TOOLCHAIN_PATH "C:/path/to/mips-elf-4.4.2")
```

### Additional CMake Build Options

#### Specifying the Generator
If your system has other build tools (e.g., Visual Studio’s `vcxproj`, Ninja), explicitly use the "Unix Makefiles" generator:
```
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/mips-toolchain.cmake ..
```

#### Parallel Building
Speed up the build process using multiple workers:

```
cmake --build . --parallel 4
```

Alternatively, set the `MAKEFLAGS` environment variable:
```
export MAKEFLAGS="-j4"
cmake --build .
```


#### Verbose Output
See detailed build commands for debugging:
```
cmake --build . --verbose
```
Equivalent to `make VERBOSE=1`.

#### Installing Artifacts
- Install the built binaries and libraries to a designated directory (default: `install/MIPS`):
  ```
  cmake --install .
  ```
  - Equivalent to `make install`.
  - Copies `MC25_telematicsM32A01.elf`, `.bin`, `.lod`, and `libadapter.a` to `install/MIPS/bin` and `install/MIPS/lib`.
