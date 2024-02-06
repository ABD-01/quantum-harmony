#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CRC_LENGTH 4

// write a function that takes in a buffer and a size, outbuffer and n and removed last n bytes from the buffer and stores in outbuffer
void trimLastNBytes(char *data, int *length, char *lastBytes, int n)
{
    if ((data != NULL) && (length != NULL) && (lastBytes != NULL))
    {
        if (*length > n)
        {
            memmove(lastBytes, lastBytes + n, 4 - n);
            memcpy(lastBytes + 4 - n, data + (*length - n), n);
            *length -= n;
        }
        else // take whatever is there into the lastBytes
        {
            memmove(lastBytes, lastBytes + *length, 4 - *length);
            memcpy(lastBytes + 4 - *length, data, *length);
            *length = 0;
        }
    }
}

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

void printBuffer(char *buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%02hhX ", buffer[i]);
    }
    printf("\n");
    memset (buffer, 0, size);
}



int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <file_name>\r\n", argv[0]);
        return 1;
    }
    char buffer[1024] = {0};
    long fileSize = 0;
    long remaining, offset;
    long actualSize;
    int read;
    uint32_t runningFileCRC = 0xFFFFFFFF;
    uint16_t chunkCRC = 0x00;
    int count = 0;

    unsigned char CRC[4] = {0};

    FILE *f = fopen (argv[1], "rb");
    if (f)
    {
        fseek (f, 0, SEEK_END);
        fileSize = ftell (f);
        fseek (f, 0, SEEK_SET);

        printf("File Size is:  %ld\n", fileSize);

        printf("Running File CRC is: %08X\r\n", runningFileCRC);

        remaining = fileSize;
        offset = 0;

        actualSize = fileSize - CRC_LENGTH;

        while (remaining > 0)
        {
            memset(buffer, 0x00, 1024);
            
            read = fread (buffer, 1, 1024, f);
            runningFileCRC = crc_calc32(runningFileCRC, buffer, read);
            // printf("Running File CRC is: %08X\r\n", runningFileCRC);

            chunkCRC = crc_calc16(0x00, buffer, read);
            printf("Chunk CRC: %04X\n", chunkCRC);

#if !defined (NOTRIM)
            if (offset+read > actualSize)
            {
                printf("This buffer may contain CRC bytes\n");

                int numCRCbytes = offset+read - actualSize;
                printf("Number of CRC bytes: %d\n", numCRCbytes);
                int len = read;
                trimLastNBytes(buffer, &len, CRC, numCRCbytes);
                printf ("CRC is: ");
                for (int i = 0; i < 4; i++)
                    printf ("%02hhX ", CRC[i]);
                printf("\n");
                printBuffer(buffer, read);

                uint32_t extractedCrc;
                extractedCrc = (uint32_t)CRC[0] << 24 | (uint32_t)CRC[1] << 16 | (uint32_t)CRC[2] << 8 | (uint32_t)CRC[3];

                printf("CRC is : %lu\n", extractedCrc);
                printf("CRC is : %08X\n", extractedCrc);
            }
#endif

            offset += read;
            remaining -= read;
            // printf("Offset: %ld, Read: %d, Remaining: %ld\n", offset, read, remaining);
            count ++;
        }
    }
    // printf("Cound: %d\n", count);
    printf ("\n");
    return 0;
}