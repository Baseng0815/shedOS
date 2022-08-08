#ifndef _MEMUTIL_H
#define _MEMUTIL_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const void*, const void*, size_t);
void *memcpy(void*, const void*, size_t);

void *memcpy_128(void*, const void*, size_t);
void *memset(void*, uint8_t, size_t);

#endif
