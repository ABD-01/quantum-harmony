/**
 * @file        src/hexdump.c
 *
 * @copyright   Accolade Electronics Pvt Ltd, 2024
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 * @date        February 6, 2024
 * @author      Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 * 
 * @brief       This file contains the implementation of hexdump functions
 */

#include "hexdump.h"
#include "bn_logger.h"

void hexdump(const void *buffer, size_t size) {
    const unsigned char *buf = (const unsigned char *)buffer;

    for (size_t i = 0; i < size; ++i) {
        bn_logger("%02X ", buf[i]);

        // Add a newline after every NUMCOLS bytes for better readability
        if ((i + 1) % NUMCOLS == 0 || i == size - 1) {
            for (size_t j = 0; j < NUMCOLS - (i % NUMCOLS); ++j) {
                bn_logger("%s","   "); // Add extra spaces for alignment
            }
            bn_logger("%s", "| ");
            for (size_t j = i - (i % NUMCOLS); j <= i; ++j) {
                if (buf[j] >= ' ' && buf[j] <= '~') {
                    bn_logger("%c", buf[j]);
                } else {
                    bn_logger("%s",".");
                }
            }
            bn_logger("%s",""LINEEND"");
        }
    }
    bn_logger("%s", ""LINEEND"");
}

void hexdumpToBuffer(const void *buffer, size_t size, char* outBuffer) {
    const unsigned char *buf = (const unsigned char *)buffer;
    int offset = 0;

    for (size_t i = 0; i < size; ++i) {
        offset += sprintf(outBuffer + offset, "%02X ", buf[i]);

        // Add a newline after every NUMCOLS bytes for better readability
        if ((i + 1) % NUMCOLS == 0 || i == size - 1) {
            for (size_t j = 0; j < NUMCOLS - (i % NUMCOLS); ++j) {
                offset += sprintf(outBuffer + offset, "   "); // Add extra spaces for alignment
            }
            offset += sprintf(outBuffer + offset, "| ");
            for (size_t j = i - (i % NUMCOLS); j <= i; ++j) {
                if (buf[j] >= ' ' && buf[j] <= '~') {
                    offset += sprintf(outBuffer + offset, "%c", buf[j]);
                } else {
                    offset += sprintf(outBuffer + offset, ".");
                }
            }
            offset += sprintf(outBuffer + offset, ""LINEEND"");
        }
    }
}

//  cl .\hexdump\hexdump.c /I .\hexdump\ /c /Fo.\hexdump\hexdump.o 
//  clang -c .\hexdump\hexdump.c -o .\hexdump\hexdump.o
//  gcc -c hexdump/hexdump.c -o hexdump/hexdump.o