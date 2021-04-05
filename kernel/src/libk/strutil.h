#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <stdint.h>
#include <stddef.h>

size_t strlen(const char*);

/* unsigned integer to string conversion */
size_t utos(uint64_t, uint64_t, char*);

#endif
