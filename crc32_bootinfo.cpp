/**
 * @file    crc32_bootinfo.cpp
 * @author  Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 * @date    14 April 2025
 * @version 1.0
 * @brief   Appends CRC32 checksum to a binary file (if needed) and generates a boot_info_data.c
 *          with embedded metadata, including the computed CRC and file size.
 *
 * Copyright (c) 2024-2025 Accolade Electronics Pvt. Ltd. All Rights Reserved.
 *
 * Changelog:
 *
 * 2025-04-30   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - Bugfix: Using `unsigned char` to deal with raw file buffer.(converting signed char to 32 bit
 *     int is unsafe)
 *   - Refactored crc32 function as a template function
 *
 * 2025-04-29   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - Added additional fields in struct: update_source, update_type, backup_pending
 *
 * 2025-04-17   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - Add CRC32 calculation for boot info buffer to ensure data integrity verification
 *
 * 2025-04-16   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - File size in boot_info_data.c will not account for the CRC bytes
 *
 * 2025-04-15   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - Updated BootInfo_t structure. Added fields: debug, curr_retries, prev_retries
 *
 * 2025-04-14   Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 *   - Initial creation of the file.
 *
 */

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;

constexpr uint32_t POLYNOMIAL = 0x04C11DB7;

constexpr auto crc_table = [] {
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

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <file>" << endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        cerr << "Unable to open file " << argv[1] << endl;
        return 1;
    }
    size_t file_size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    cout << "File Size (Before): " << file_size << endl;

    // std::vector<char> buffer(file_size);
    // if(!file.read(reinterpret_cast<char*>(buffer.data()), file_size))
    auto *buffer = new unsigned char[file_size];
    if (!file.read(reinterpret_cast<char *>(buffer), file_size)) {
        cerr << "Unable to read file " << argv[1] << endl;
        delete[] buffer;
        return 1;
    }
    file.close();

    uint32_t crc = crc32(buffer, file_size);

    bool skip_append = false;
    uint32_t crc_value = crc;

    if (file_size >= 4 && crc == 0) [[unlikely]] {
        uint32_t existing_crc = static_cast<uint32_t>(buffer[file_size - 4]) << 24;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 3]) << 16;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 2]) << 8;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 1]);

        uint32_t recomputed_crc = crc32(buffer, file_size - 4);
        if (existing_crc == recomputed_crc) {
            crc_value = existing_crc;
            cout << "File CRC already exists as the trailing 4 bytes" << endl;
            cout << "File CRC: 0x" << std::hex << std::uppercase << crc_value << endl;
            skip_append = true;
        }
    }

    if (!skip_append) [[likely]] {
        ofstream outfile(argv[1], std::ios::binary | std::ios::app);
        if (!outfile.is_open()) {
            cerr << "Unable to open file for append " << argv[1] << endl;
            delete[] buffer;
            return 1;
        }
        char crc_bytes[4] = {static_cast<char>((crc_value >> 24) & 0xFF),
                             static_cast<char>((crc_value >> 16) & 0xFF),
                             static_cast<char>((crc_value >> 8) & 0xFF),
                             static_cast<char>(crc_value & 0xFF)};
        if (!outfile.write(crc_bytes, 4 * sizeof(char))) {
            cerr << "Unable to append to file " << argv[1] << endl;
            delete[] buffer;
            return 1;
        }
        outfile.close();
        cout << "File CRC: 0x" << std::hex << std::uppercase << crc_value << endl;
    }

    cout << "File Size (After): " << std::dec << file_size + (skip_append ? 0 : 4) << endl;
    delete[] buffer;

    /**
     * Creating boot_info_data.c file
     */
    generate_bootinfo_file(file_size, crc_value);

    return 0;
}

int generate_bootinfo_file(uint32_t file_size, uint32_t crc_value)
{

    ofstream boot_info_file("boot_info_data.c", std::ios::trunc);
    if (!boot_info_file.is_open()) {
        cerr << "Unable to open boot_info_data.c" << endl;
        return 1;
    }

    constexpr size_t FLASH_SECTOR_SIZE = 1024;
    union {
        struct __attribute__((__packed__)) {
            uint8_t debug;
            uint8_t update;
            uint8_t update_source;
            uint8_t update_type;
            uint8_t backup_pending;
            uint8_t curr_retries;
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
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0xA5A5A5A5, 0x00000000, 0x00009400, 0x00009000,
         0x00013000, 0x00009000, file_size, 0xFFFFFFFF, crc_value, 0xFFFFFFFF},
    };

    uint32_t block_crc32 = crc32(_boot_info.buff, FLASH_SECTOR_SIZE);

    boot_info_file << "// Automatically-generated file. Do not edit!\n" << endl;
    boot_info_file << "#include <stdint.h>" << endl;
    boot_info_file << "#define FLASH_SECTOR_SIZE 1024" << endl;
    boot_info_file << "const union {struct __attribute__((__packed__)) {"
                      "uint8_t debug;"
                      "uint8_t update;"
                      "uint8_t update_source;"
                      "uint8_t update_type;"
                      "uint8_t backup_pending;"
                      "uint8_t curr_retries;"
                      "uint16_t prev_retries;"
                      "uint32_t initialized;"
                      "uint32_t start_bl;"
                      "uint32_t start_app;"
                      "uint32_t part_size_bl;"
                      "uint32_t part_size_app;"
                      "uint32_t img_len_bl;"
                      "uint32_t img_len_app;"
                      "uint32_t crc_bl;"
                      "uint32_t crc_app;"
                      "uint32_t crc32;"
                      "} st;"
                      "uint8_t buff[FLASH_SECTOR_SIZE];"
                      "} _boot_info __attribute__((section(\".boot_info\"),used)) = {{"
                      "0x00,"
                      "0x00,"
                      "0x00,"
                      "0x00,"
                      "0x00,"
                      "0x00,"
                      "0x0000,"
                      "0xA5A5A5A5,"
                      "0x00000000,"
                      "0x00009400,"
                      "0x00009000,"
                      "0x00013000,"
                      "0x00009000,"
                      "0x"
                   << std::hex << std::uppercase << file_size
                   << ","
                      "0xFFFFFFFF,"
                      "0x"
                   << std::hex << std::uppercase << crc_value
                   << ","
                      "0x"
                   << std::hex << std::uppercase << block_crc32 << "}};" << endl;
    boot_info_file << "__attribute__((section(\".text\"))) void _dummy_entry(){}" << endl;
    boot_info_file.close();
    cout << "boot_info_data.c created successfully" << endl;

    return 0;
}

template <typename T>
crc32_result_t<T> crc32(const T *data, size_t size)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
        uint8_t index = ((crc >> 24) ^ static_cast<uint8_t>(data[i])) & 0xFF;
        crc = crc_table[index] ^ (crc << 8);
    }
    return crc;
}
