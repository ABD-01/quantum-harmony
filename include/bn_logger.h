/**
 * @file        include/bn_logger.h
 *
 * @copyright   Accolade Electronics Pvt Ltd, 2024
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 * @date        May 29, 2024
 * @author      Muhammed Abdullah Shaikh <muhammed.shaikh@accoladeelectronics.com>
 * 
 * @brief       Header File for Logger function prototype
 */

#ifndef BN_LOGGER_H
#define BN_LOGGER_H


#ifdef __cplusplus
extern "C" {
#endif

#define CR          "\r"
#define LF          "\n"
#define CRLF        "\r\n"

#define LINEEND     CRLF


/**
 * @brief Logs a message with the specified format.
 *
 * @param format The format string specifying the message to be logged.
 * @param ... The variable argument list containing the values to be formatted into the message.
 * @warning The user MUST implement this function in their codebase. The implementation should use the `vprintf` function from the standard library to print the formatted message.
 * 
 * Example Implementation:
 * @code{.c}
 * #include "bn_logger.h"
 * void bn_logger(const char *format, ...)
 * {
 *     va_list args;
 *     va_start(args, format);
 *     vprintf(format, args);
 *     va_end(args);
 * } 
 * @endcode
 */
void bn_logger(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif /* BN_LOGGER_H */