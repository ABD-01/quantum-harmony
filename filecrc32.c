#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <Windows.h>

uint32_t crc_calc32(uint32_t runningCrc, const uint8_t *const buf, size_t bufSize)
{
    size_t i;
    size_t j;
    unsigned int msb;

    for (i = 0; i < bufSize; i++)
    {
        runningCrc ^= (((unsigned int)buf[i]) << 24);
        for (j = 0; j < 8; j++)
        {
            msb = runningCrc >> 31;
            runningCrc <<= 1;
            runningCrc ^= (0 - msb) & 0x04C11DB7;
        }
    }
    return runningCrc;
}

uint16_t crc_calc16(uint16_t seed, uint8_t* src, uint32_t len)
{
    uint16_t crc = seed;
    int a = 0;
    for (; a < len; a++)
    {
        crc ^= (uint16_t) (src[a] << 8);
        for (int i = 0; i < 8; i++)
        {
            if ((crc & 0x8000) != 0)
                crc = (uint16_t) ((crc << 1) ^ 0x1021);
            else
                crc = (uint16_t) (crc << 1);
        }
    }
    return crc;
}

void printBuffer(uint8_t* src, uint32_t len)
{
    int a = 0;
    for (; a < len; a++)
    {
        printf("%02X ", src[a]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <file_name>\r\n", argv[0]);
        Sleep(5000);
        return 1;
    }
    char *buffer = 0;
    long length;
    uint32_t crc32;
    uint16_t crc16;
    FILE *f = fopen(argv[1], "rb");
 
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer)
        {
            fread(buffer, length, 1, f);
        }
        fclose(f);
    }

    // printf("\n");
    if (length < 256)
        printBuffer(buffer, length);
    printf("File Size is:  %ld\n", length);

    crc32 = crc_calc32(0xFFFFFFFF, buffer, length);
    // printf("CRC32 is:  %lu\n", crc32);
    // printf("CRC32 is:  %04X\n", crc32);
    printf("CRC32 is:  %08X\n", crc32);

    printf("\n");

    // crc16 = crc_calc16(0, buffer, length);
    // printf("CRC16 is:  %lu\n", crc16);
    // printf("CRC16 is:  %04X\n", crc16);
    // printf("CRC16 is:  %08X\n", crc16);

    free(buffer);

    return 0;
}
