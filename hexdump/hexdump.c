#include "hexdump.h"

void hexdump(const void *buffer, size_t size) {
    const unsigned char *buf = (const unsigned char *)buffer;

    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", buf[i]);

        // Add a newline after every NUMCOLS bytes for better readability
        if ((i + 1) % NUMCOLS == 0 || i == size - 1) {
            for (size_t j = 0; j < NUMCOLS - (i % NUMCOLS); ++j) {
                printf("   "); // Add extra spaces for alignment
            }
            printf("| ");
            for (size_t j = i - (i % NUMCOLS); j <= i; ++j) {
                if (buf[j] >= ' ' && buf[j] <= '~') {
                    printf("%c", buf[j]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    printf("\n");
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
            offset += sprintf(outBuffer + offset, "\n");
        }
    }
}

//  cl .\hexdump\hexdump.c /I .\hexdump\ /c /Fo.\hexdump\hexdump.o 
//  clang -c .\hexdump\hexdump.c -o .\hexdump\hexdump.o
//  gcc -c hexdump/hexdump.c -o hexdump/hexdump.o