#include <stdarg.h>
#include <string.h>

#include "unity.h"
#include "bn_logger.h"
#include "hexdump.h"

// Buffer to capture log output
static char log_buffer[1024];
static size_t log_buffer_offset = 0;

// Mock bn_logger function
// void my_logger(const char *format, va_list args) {
//     log_buffer_offset += vsprintf(log_buffer + log_buffer_offset, format, args);
// }

void bn_logger(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_buffer_offset += vsprintf(log_buffer + log_buffer_offset, format, args);    va_end(args);
}


// Reset log buffer before each test
void setUp(void) {
    log_buffer[0] = '\0';
    log_buffer_offset = 0;
    // set_log_function(my_logger);
}


void tearDown(void) {
    // Clean up code, if needed
}

void test_hexdump(void) {
    const char *data = "Hello, World!";
    size_t size = strlen(data);

    hexdump(data, size);

    const char *expected_output = 
        "48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21                                     | Hello, World!\r\n"
        "\r\n";

    TEST_ASSERT_EQUAL_STRING(expected_output, log_buffer);
}

void test_hexdumpToBuffer(void) {
    const void *buffer = "Hello, World!";
    size_t size = 13;
    char outBuffer[512]; // Adjust the size as needed
    hexdumpToBuffer(buffer, size, outBuffer);
    bn_logger("%s", outBuffer);
    // Add your assertions here to verify the output in outBuffer
    TEST_ASSERT_EQUAL_STRING("48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21             | Hello, World!\n", outBuffer);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hexdump);
    // RUN_TEST(test_hexdumpToBuffer);
    return UNITY_END();
}
