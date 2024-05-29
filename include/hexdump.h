#ifndef HEXDUMP_H
#define HEXDUMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#if !defined(NUMCOLS)
#define NUMCOLS             24
#endif

void hexdump(const void *buffer, size_t size);
void hexdumpToBuffer(const void *buffer, size_t size, char* outBuffer);

#ifdef __cplusplus
}
#endif

#endif // HEXDUMP_H

