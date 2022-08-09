#ifndef _MEMUTIL_H
#define _MEMUTIL_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const void* p1, const void *p2, size_t n);
void *memcpy(void *dst, const void *src, size_t n);

void *memcpy_128(void *dst, const void *src, size_t n);
void *memset(void *dst, uint8_t c, size_t n);

#endif
