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

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cstdint>

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;

constexpr uint32_t POLYNOMIAL = 0x04C11DB7;

constexpr auto crc_table = [] {
    std::array<uint32_t, 256> table{};
    for(uint32_t i = 0; i < 256; i++)
    {
        uint32_t crc = i << 24;
        for(int j = 0; j < 8; j++)
            crc = (crc << 1) ^ (crc & 0x80000000 ? POLYNOMIAL : 0);
        table[i] = crc;
    }
    return table;
}();

uint32_t crc32(const std::vector<char>& data);
uint32_t crc32(const char * data, size_t size);


int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <file>" << endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        cerr << "Unable to open file " << argv[1] << endl;
        return 1;
    }
    size_t file_size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    cout << "File Size (Before): " << file_size << endl;
    

    // std::vector<char> buffer(file_size);
    // if(!file.read(reinterpret_cast<char*>(buffer.data()), file_size))
    char* buffer = new char[file_size];
    if(!file.read(buffer, file_size))
    {
        cerr << "Unable to read file " << argv[1] << endl;
        delete[] buffer;
        return 1;
    }
    file.close();

    uint32_t crc = crc32(buffer, file_size);

    bool skip_append = false;
    uint32_t crc_value = crc;

    if (file_size >= 4 && crc == 0)
    {
        uint32_t existing_crc  = static_cast<uint32_t>(buffer[file_size - 4]) << 24;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 3]) << 16;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 2]) << 8;
        existing_crc |= static_cast<uint32_t>(buffer[file_size - 1]);

        uint32_t recomputed_crc = crc32(buffer, file_size - 4);
        if(existing_crc == recomputed_crc)
        {
            crc_value = existing_crc;
            cout << "File CRC already exists as the trailing 4 bytes" << endl;
            cout << "File CRC: 0x" << std::hex << std::uppercase << crc_value << endl;
            skip_append = true;
        }
    }

    if (!skip_append)
    {
        ofstream outfile(argv[1], std::ios::binary | std::ios::app);
        if(!outfile.is_open())
        {
            cerr << "Unable to open file for append " << argv[1] << endl;
            delete[] buffer;
            return 1;
        }
        char crc_bytes[4] = {
            static_cast<char>((crc_value >> 24) & 0xFF),
            static_cast<char>((crc_value >> 16) & 0xFF),
            static_cast<char>((crc_value >> 8) & 0xFF),
            static_cast<char>(crc_value & 0xFF)
        };
        if(!outfile.write(crc_bytes, 4 * sizeof(char)))
        {
            cerr << "Unable to append to file " << argv[1] << endl;
            delete[] buffer;
            return 1;
        }
        outfile.close();
        cout << "File CRC: 0x" << std::hex << std::uppercase << crc_value << endl;
        file_size += 4;
    }

    cout << "File Size (After): " << std::dec << file_size << endl;
    delete[] buffer;
    
    /**
     * Creating boot_info_data.c file
     */
    ofstream boot_info_file("boot_info_data.c", std::ios::trunc);
    if(!boot_info_file.is_open())
    {
        cerr << "Unable to open boot_info_data.c" << endl;
        return 1;
    }
    boot_info_file << "// Automatically-generated file. Do not edit!\n" << endl;
    boot_info_file << "#include <stdint.h>" << endl;
    boot_info_file << "#define APP_START_ADDR 0x00009400" << endl;
    boot_info_file << "typedef union{struct __attribute__((__packed__)){uint8_t img_update;uint8_t debug;uint8_t curr_retries;uint16_t prev_retries;uint32_t initialized;uint32_t start_bl;uint32_t start_app;uint32_t part_size_bl;uint32_t part_size_app;uint32_t img_len_bl;uint32_t img_len_app;uint32_t crc_bl;uint32_t crc_app;uint32_t crc32;}st;uint8_t buff[1024];} BootInfo_t;" << endl;
    boot_info_file << "const BootInfo_t _boot_info __attribute__((section(\".boot_info\"),used)) = {{"
                      "0x00,"
                      "0x00,"
                      "0x00,"
                      "0x0000,"
                      "0xA5A5A5A5,"
                      "0x00000000,"
                      "APP_START_ADDR,"
                      "0x00009000,"
                      "0x00013000,"
                      "0x00000000,"
                      "0x" << std::hex << std::uppercase << file_size - (skip_append ? 0 : 4) << ","
                      "0x00000000,"
                      "0x" << std::hex << std::uppercase << crc_value << ","
                      "0xFFFFFFFF"
                      "}};" << endl;
    boot_info_file << "__attribute__((section(\".text\"))) void _dummy_entry(){}" << endl;
    boot_info_file.close();
    cout << "boot_info_data.c created successfully" << endl;

    return 0;
}

uint32_t crc32(const std::vector<char>& data)
{
    uint32_t crc = 0xFFFFFFFF;
    for(const char& byte : data)
    {
        uint8_t index = ((crc >> 24) ^ static_cast<uint8_t>(byte)) & 0xFF;
        crc = crc_table[index] ^ (crc << 8);
    }
    return crc;
}

uint32_t crc32(const char * data, size_t size)
{
    uint32_t crc = 0xFFFFFFFF;
    for(size_t i = 0; i < size; ++i)
    {
        uint8_t index = ((crc >> 24) ^ static_cast<uint8_t>(data[i])) & 0xFF;
        crc = crc_table[index] ^ (crc << 8);
    }
    return crc;
}
