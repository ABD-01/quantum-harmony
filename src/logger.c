#include <stdio.h>
#include <stdarg.h>

#include "bn_logger.h"

static log_function_t user_log_function = NULL;

void set_log_function(log_function_t function) {
    user_log_function = function;
}

void bn_logger(const char *format, ...) {
    if (user_log_function != NULL) {
        va_list args;
        va_start(args, format);
        user_log_function(format, args);
        va_end(args);
    }
}