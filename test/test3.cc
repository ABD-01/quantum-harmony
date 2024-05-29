#include <gtest/gtest.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

extern "C" {
    #include "hexdump.h"
    #include "bn_logger.h"
}

// Mock log buffer and logger function
static char log_buffer[1024];
static size_t log_buffer_offset = 0;

void my_logger_v(const char *format, va_list args) {
    log_buffer_offset += vsprintf(log_buffer + log_buffer_offset, format, args);
}

void bn_logger(const char *format, ...) {
    va_list args;
    va_start(args, format);
    my_logger_v(format, args);
    va_end(args);
}

// Test fixture for setting up and tearing down
class HexdumpTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset log buffer before each test
        log_buffer[0] = '\0';
        log_buffer_offset = 0;
        // set_log_function(my_logger);
    }

    void TearDown() override {
        // Clean up code if needed
    }
};

TEST_F(HexdumpTest, ConvertsToHexCorrectly) {
    const char *data = "Hello, World!";
    size_t size = strlen(data);

    hexdump(data, size);

    const char *expected_output = 
        "48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21                                     | Hello, World!\r\n"
        "\r\n";

    EXPECT_STREQ(expected_output, log_buffer);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
