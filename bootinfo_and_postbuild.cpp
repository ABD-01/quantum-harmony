/**
 * @file    bootinfo_and_postbuild.cpp
 * @author  Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 * @date    14 April 2025
 * @version 2.0
 * @brief   Appends CRC32 checksum to a binary file (if needed) and generates a boot_info_data.c
 *          with embedded metadata, including the computed CRC and file size.
 *
 * Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
 *
 * Changelog:
 *
 * 2025-05-28   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - (v2.0) Appends a fixed-length (28-byte) application version before the CRC32 bytes in the
 * binary image. [   APPLICATION        |    VERSION (28 bytes)   |   CRC32 (4 bytes)  ]
 *   - Generates a new binary file instead of modifying the input file in-place.
 *   - Skips genearting a new file, if the input file already contains CRC. (It is assumed
 *     that such file also includes version bytes.)
 *   - The srec command file generation is now limited to the hex file.
 *
 * 2025-05-20   Muhammed Abdullah Shaikh
 *   - Bootloader length and app start region updated
 *   - Initial CRC of block set to `0`.
 *
 * 2025-05-02   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - (v1.1) Added generate_appcrc_file and generate_srec_cmd_file functions
 *
 * 2025-04-30   Muhammed Abdullah Shaikh
 *   - Bugfix: Using `unsigned char` to deal with raw file buffer.(converting signed char to 32 bit
 *     int is unsafe)
 *   - Refactored crc32 function as a template function
 *
 * 2025-04-29   Muhammed Abdullah Shaikh
 *   - Added additional fields in struct: update_source, update_type, backup_pending
 *
 * 2025-04-17   Muhammed Abdullah Shaikh
 *   - Add CRC32 calculation for boot info buffer to ensure data integrity verification
 *
 * 2025-04-16   Muhammed Abdullah Shaikh
 *   - File size in boot_info_data.c will not account for the CRC bytes
 *
 * 2025-04-15   Muhammed Abdullah Shaikh
 *   - Updated BootInfo_t structure. Added fields: debug, curr_retries, prev_retries
 *
 * 2025-04-14   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - (v1.0) Initial creation of the file.
 *
 */

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

constexpr uint32_t POLYNOMIAL = 0x04C11DB7;
constexpr auto     crc_table  = [] {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i << 24;
        for (int j = 0; j < 8; j++)
            crc = (crc << 1) ^ (crc & 0x80000000 ? POLYNOMIAL : 0);
        table[i] = crc;
    }
    return table;
}();

template <typename T>
using crc32_result_t = typename std::enable_if<std::is_integral<T>::value, uint32_t>::type;

template <typename T>
crc32_result_t<T> crc32(const T *data, size_t size);

int generate_bootinfo_file(uint32_t, uint32_t);
int generate_appcrc_file(uint32_t);
int generate_srec_cmd_file(uint32_t, std::string);

constexpr unsigned CRC32_SIZE       = 4;
constexpr unsigned VERSION_SIZE     = 28;
constexpr uint32_t APP_START_REGION = 0x0000A400;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file> [version]" << endl;
        return 1;
    }

    std::string      filename = argv[1];
    std::string_view app_version =
        (argc == 3 && strlen(argv[2]) > 0) ? argv[2] : "A2TP_0.0.0_TST00";
    if (app_version.length() > VERSION_SIZE) {
        cerr << "Version size cannot be greater than " << VERSION_SIZE << endl;
        return 1;
    }

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        cerr << "Unable to open file " << filename << endl;
        return 1;
    }
    size_t file_size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    cout << "File Size: " << file_size << endl;

    auto *buffer = new unsigned char[file_size + VERSION_SIZE + CRC32_SIZE];
    if (!file.read(reinterpret_cast<char *>(buffer), file_size)) {
        cerr << "Unable to read file " << filename << endl;
        delete[] buffer;
        return 1;
    }
    file.close();

    uint32_t crc_value = crc32(buffer, file_size);

    bool skip_append = false;

    if (file_size >= 4 && crc_value == 0) [[unlikely]] {
        uint32_t existing_crc = static_cast<uint32_t>(buffer[file_size - 4]) << 24;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 3]) << 16;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 2]) << 8;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 1]);

        uint32_t recomputed_crc = crc32(buffer, file_size - 4);
        if (existing_crc == recomputed_crc) {
            crc_value = existing_crc;
            cout << "File CRC already exists as the trailing 4 bytes" << endl;
            skip_append = true;
            file_size -= 4;
        }
    }

    std::string new_filename = filename.substr(0, filename.length() - 4) + "_ver_and_appcrc.bin";
    if (!skip_append) [[likely]] {

        // Version
        std::memset(buffer + file_size, 0x00, VERSION_SIZE);
        std::memcpy(buffer + file_size, app_version.data(), app_version.size());
        file_size += VERSION_SIZE;
        cout << "App version: " << app_version << endl;

        // CRC32
        crc_value             = crc32(buffer, file_size);
        buffer[file_size]     = static_cast<unsigned char>((crc_value >> 24) & 0xFF);
        buffer[file_size + 1] = static_cast<unsigned char>((crc_value >> 16) & 0xFF);
        buffer[file_size + 2] = static_cast<unsigned char>((crc_value >> 8) & 0xFF);
        buffer[file_size + 3] = static_cast<unsigned char>(crc_value & 0xFF);
        // Dev Note: file_size not to be updated for CRC bytes
        cout << "App CRC: 0x" << std::hex << std::uppercase << crc_value << std::dec << endl;

        std::ofstream outfile(new_filename, std::ios::binary | std::ios::trunc);
        if (!outfile.is_open()) {
            cerr << "Unable to open file for append " << new_filename << endl;
            delete[] buffer;
            return 1;
        }
        if (!outfile.write(reinterpret_cast<char const *>(buffer), file_size + CRC32_SIZE)) {
            cerr << "Unable to append to file " << new_filename << endl;
            delete[] buffer;
            return 1;
        }
        cout << new_filename << " created with size " << file_size + CRC32_SIZE << endl;
        outfile.close();
    }

    delete[] buffer;

    /**
     * Creating boot_info_data.c file
     */
    generate_bootinfo_file(file_size, crc_value);

    /**
     * Creating app_crc32.bin file
     */
    generate_appcrc_file(crc_value);

    /**
     * Creating srec_cmd_create_hex.txt file
     */
    generate_srec_cmd_file(file_size, new_filename);

    return 0;
}

int generate_bootinfo_file(uint32_t file_size, uint32_t app_crc_value)
{

    std::ofstream boot_info_file("boot_info_data.c", std::ios::trunc);
    if (!boot_info_file.is_open()) {
        cerr << "Unable to create boot_info_data.c" << endl;
        return 1;
    }

    constexpr size_t   FLASH_SECTOR_SIZE = 0x400;
    constexpr uint32_t BOOTLOADER_SIZE   = 0x0000A000;
    constexpr uint32_t APPLICATION_SIZE  = 0x00012000;
    union {
        struct __attribute__((__packed__)) {
            uint8_t  debug;
            uint8_t  update;
            uint8_t  update_source;
            uint8_t  update_type;
            uint8_t  backup_pending;
            uint8_t  curr_retries;
            uint16_t prev_retries;
            uint32_t initialized;
            uint32_t start_bl;
            uint32_t start_app;
            uint32_t part_size_bl;
            uint32_t part_size_app;
            uint32_t img_len_bl;
            uint32_t img_len_app;
            uint32_t crc_bl;
            uint32_t crc_app;
            uint32_t crc32;
        } st;
        uint8_t buff[FLASH_SECTOR_SIZE];
    } _boot_info = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0xA5A5A5A5, 0x00000000, APP_START_REGION,
         BOOTLOADER_SIZE, APPLICATION_SIZE, BOOTLOADER_SIZE, file_size, 0xFFFFFFFF, app_crc_value,
         0x00},
    };

    uint32_t block_crc32 = crc32(_boot_info.buff, FLASH_SECTOR_SIZE);
    _boot_info.st.crc32  = block_crc32;

    boot_info_file << "// Automatically-generated file. Do not edit!\n"
                   << "#define FLASH_SECTOR_SIZE (1024)\n"
                   << "unsigned char const _boot_info[FLASH_SECTOR_SIZE] "
                   << "__attribute__((section(\".boot_info\"),used)) = {\n";

    for (size_t i = 0; i < FLASH_SECTOR_SIZE; ++i) {
        boot_info_file << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                       << static_cast<int>(_boot_info.buff[i]) << ", ";
        if ((i + 1) % 12 == 0) boot_info_file << "\n";
    }
    boot_info_file << "\n};\n";

    boot_info_file << "__attribute__((section(\".text\"))) void _dummy_entry(){}" << endl;
    boot_info_file.close();
    cout << "boot_info_data.c created successfully" << endl;

    return 0;
}

int generate_appcrc_file(uint32_t crc_value)
{
    /**
     * Dev Note:
     * This function generates `app_crc32.bin` for appending to the firmware hex image.
     *
     * The same result can also be achieved by using `srec_cat`
     *
     *  Example:
     *    APP_START = 0x9400, APP_LENGTH = 0x4D70 → CRC address = 0xE170
     *
     *    srec_cat ti_firmware.hex -Intel \
     *        -Bit_Reverse \
     *        -CRC32_Little_Endian 0xE170 \
     *        -Bit_Reverse \
     *        -XOR 0xFF \
     *        -crop 0xE170 0xE174 \
     *        -Output app_crc32-mpeg2.hex
     *
     * This calculates a CRC32 MPEG-2 over the entire application and places the result at
     * 0xE170–0xE173.
     */

    std::ofstream crcfile("app_crc32.bin", std::ios::binary | std::ios::trunc);
    if (!crcfile.is_open()) {
        cerr << "Unable to create app_crc32.bin" << endl;
        return 1;
    }
    crc_value = __builtin_bswap32(crc_value);
    if (!crcfile.write(reinterpret_cast<const char *>(&crc_value), 4 * sizeof(char))) {
        cerr << "Unable to write to file app_crc32.bin" << endl;
        return 1;
    }
    cout << "app_crc32.bin created successfully" << endl;
    return 0;
}

int generate_srec_cmd_file(uint32_t file_size, std::string filename)
{

    const char hex_cmd_file[] = "srec_cmd_create_hex.txt";
    const char header[]       = "# Automatically-generated file.\n\n";

    std::ofstream cmd_file(hex_cmd_file, std::ios::trunc);
    if (!cmd_file.is_open()) {
        cerr << "Unable to create " << hex_cmd_file << endl;
        return 1;
    }
    cmd_file << header << filename << " -Binary "
             << "-offset 0x" << std::hex << std::uppercase << APP_START_REGION << " -Output "
             << filename.substr(0, filename.length() - 4) + ".hex" << " -Intel" << endl;
    cmd_file.close();

    cout << "srec command files created successfully" << endl;
    return 0;
}

template <typename T>
crc32_result_t<T> crc32(const T *data, size_t size)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
        uint8_t index = ((crc >> 24) ^ static_cast<uint8_t>(data[i])) & 0xFF;
        crc           = crc_table[index] ^ (crc << 8);
    }
    return crc;
}
