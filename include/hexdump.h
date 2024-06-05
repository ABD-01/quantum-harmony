/**
 * @file hexdump.h
 * @brief Hexdump implementation
 * @author Muhammed Abdullah
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
 * @brief Prints the buffer in hexdump format
 * @param buffer Input buffer
 * @param size Size
 */
void hexdump(const void *buffer, size_t size);


/**
 * @brief Prints the buffer in hexdump format to a buffer
 * @param buffer 
 * @param size 
 * @param outBuffer 
 * @see hexdump
 */
void hexdumpToBuffer(const void *buffer, size_t size, char* outBuffer);

#ifdef __cplusplus
}
#endif

#endif // HEXDUMP_H

