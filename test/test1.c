#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hexdump.h"
#include "bn_logger.h"

void bn_logger(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

typedef struct 
{
    uint16_t lenght;
    uint8_t  data[512];
} data_s;


static uint32_t g_randSeed = 42;
static void generatRandomBuffer(uint8_t buffer[], size_t size);


int main(int argc, char* argv[]) {

    // set_log_function(my_logger);

    data_s data = { 0 };
    data.lenght = 100;

    generatRandomBuffer(data.data, data.lenght);

    hexdump(data.data, data.lenght);
    // char outBuffer[500];

    // hexdumpToBuffer(data.data, data.lenght, outBuffer);

    // printf("%s\n", outBuffer);


    bn_logger("Test message: %s\n", "Hello, World!");

    return 0;
}

static void generatRandomBuffer(uint8_t buffer[], size_t size)
{
    uint32_t *ptr = (uint32_t *)buffer;
    size_t numIterations = size / sizeof(uint32_t);

    for (size_t i = 0; i < numIterations; ++i) {
    	g_randSeed ^= (g_randSeed << 13);
    	g_randSeed ^= (g_randSeed >> 17);
    	g_randSeed ^= (g_randSeed << 5);
        ptr[i] = g_randSeed;
    }
}