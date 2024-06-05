/**
 * @file        include/hexdump.h
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
 * @brief       Header File for hexdump functions
 */
#ifndef HEXDUMP_H
#define HEXDUMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#if !defined(NUMCOLS)
 /**
  * @brief Width of hexdump
 */
#define NUMCOLS             24
#endif


/**
 * @brief Prints the buffer in hexdump format. Depends on @ref bn_logger
 * @param buffer Input buffer
 * @param size Size
 */
void hexdump(const void *buffer, size_t size);


/**
 * @brief Prints the buffer in hexdump format to a buffer
 * @param buffer input buffer
 * @param size size of buffer
 * @param outBuffer buffer to store the hexdump output
 * @see hexdump
 */
void hexdumpToBuffer(const void *buffer, size_t size, char* outBuffer);

#ifdef __cplusplus
}
#endif

#endif // HEXDUMP_H

