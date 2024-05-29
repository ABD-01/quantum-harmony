#ifndef BN_LOGGER_H
#define BN_LOGGER_H


#define CR          "\r"
#define LF          "\n"
#define CRLF        "\r\n"

#define LINEEND     CRLF

typedef void (*log_function_t)(const char *format, ...);

void set_log_function(log_function_t);
void bn_logger(const char *format, ...);

#endif /* BN_LOGGER_H */