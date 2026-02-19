<!--
 * @file    README.md
 * @author  Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 * @date    21 July 2025
 * @version 1.0
 * @brief   Documentation for Firmware Boot Information and Post-Build Processing System.
 *          Describes components, build steps, structure layout, and integration instructions.
 *
 * Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
 *
 * Changelog:
 * 2025-07-21   Muhammed Abdullah Shaikh
 *    - Initial version of README with full system overview and setup instructions
-->

# Firmware Boot Information and Post-Build Processing System

## Key Components

### Core Files
- `bootinfo_and_postbuild.cpp`: Main utility that 
    - Append version & CRC32
    - Generate `.c` image files
    - Output new `.bin/.hex`
- `boot_info.mk`: Makefile for boot information generation
- `hex_and_bin.mk`: Makefile for hex/bin conversion
- `linker.cmd`: Linker script for boot info memory mapping
- `generate_bootinfo.sln`: Visual Studio solution for building the utility

### Generated Outputs
- `ti_firmware_ver_and_appcrc.bin`: App Image with 28-byte version + 4-byte CRC32 (MPEG2)
- `ti_firmware_ver_and_appcrc.hex`: Intel Hex equivalent 
- `boot_info_image.c`: Boot metadata as a C array (`.boot_info_image` section)
- `app_image.c`: Binary application as C array (`.app_image` section)
- `boot_info.bin` + `.hex`: Boot information sector

## Build Process

### 1. Build `generate_bootinfo_file.exe`
Choose one method:
```bash
# Option 1: Make
make

# Option 2: Visual Studio
Open generate_bootinfo.sln in Visual Studio

# Option 3: Command line
msbuild /p:Configuration=Release generate_bootinfo.sln
```

### Copy these files to your post_build_pipeline folder:
- `generate_bootinfo_file.exe`
- `boot_info.mk`
- `hex_and_bin.mk`
- `linker.cmd`

### 2. Set Up Post-Build Steps

In you IDE, go to project settings, add post build steps.

#### 1. Arm Bin Utility

command=`"${CG_TOOL_MAKE} -f ../post_build_pipeline/hex_and_bin.mk"`

command line options=`"-k all"`

Input Files=`"${BuildArtifactFileName}"`

Output Files=`"${BuildArtifactFileBaseName}.bin"` and `"${BuildArtifactFileBaseName}.hex"`


#### 2. Post Build Pipeline

command=`"${CG_TOOL_MAKE} -f ../post_build_pipeline/boot_info.mk"`

command line options=`"-k -j 8 all -O"`

Input Files=`"${BuildArtifactFileBaseName}.bin"`

Output Files=`"post_build_pipeline/boot_info.hex"`

## Version & CRC Format

`ti_firmware_ver_and_appcrc.bin` layout:

```
[ App Binary ][ 28-byte ASCII version ][ 4-byte CRC32 ]
```

> CRC32: MPEG-2 variant, Big Endian, computed over App+Version.

## Boot Information Structure

Located in `FLASH_SECTOR_SIZE` (0x400) memory sector:

```
union {
    PACK(struct {
        uint8_t  debug;           // Debug state
        uint8_t  update;          // App update triggered
        uint8_t  update_source;   // Source of update
        uint8_t  update_type;     // Type of update
        uint8_t  backup_pending;  // Pending backup flag
        uint8_t  curr_retries;    // Retries for current image
        uint16_t prev_retries;    // Retries for previous image
        uint32_t initialized;     // Magic (0xA5A5A5A5 = valid)
        uint32_t start_bl;        // Bootloader start address
        uint32_t start_app;       // Application start address
        uint32_t part_size_bl;    // Partition size for bootloader
        uint32_t part_size_app;   // Partition size for app
        uint32_t img_len_bl;      // Actual bootloader image length
        uint32_t img_len_app;     // Actual app image length (excluding CRC)
        uint32_t crc_bl;          // CRC32 of bootloader (0xFFFFFFFF default)
        uint32_t crc_app;         // CRC32 of app image (computed)
        uint32_t crc32;           // CRC32 of this entire 1KB block
    }) st;

    uint8_t buff[1024]; // Total structure size = 0x400 bytes (1 flash sector)
} _boot_info;
```

## Maintenance Notes
- Keep **changelog** updated in each modified file. Bump version numbers accordingly
- CRC algorithm (32 Bit MPEG2) must match the one used by the bootloader.
- Memory addresses in `linker.cmd` must match specifications from application.
- Boot info structure size must not exceed `FLASH_SECTOR_SIZE`

## License
(c) 2024–2025 Accolade Electronics Pvt. Ltd.

## Temp (Dev Note) The entire activity via command line

```sh
srec_cat ti_firmware.hex -Intel -Bit_Reverse  -CRC32_Little_Endian 0xE170 -Bit_Reverse -XOR 0xFF -crop 0xE170 0xE174 -Output firmware_3.hex -Intel
srec_cat ti_firmware.hex -Intel app_crc32.hex -Intel -Output firmware_3.hex -Intel
srec_cat firmware_3.hex -Intel -offset -0x00009400 -o firmware_5.bin -Binary

#  ti_firmware.hex -Intel -offset -0x00009400 crc32.bin -Binary -offset 19824  -o firmware_3.bin -Binary
```
