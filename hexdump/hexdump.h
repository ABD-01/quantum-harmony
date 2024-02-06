#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>

#define NUMCOLS             24

void hexdump(const void *buffer, size_t size);

#endif // HEXDUMP_H

