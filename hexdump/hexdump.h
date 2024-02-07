#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>

#if !defined(NUMCOLS)
#define NUMCOLS             24
#endif

void hexdump(const void *buffer, size_t size);
void hexdumpToBuffer(const void *buffer, size_t size, char* outBuffer);

#endif // HEXDUMP_H

