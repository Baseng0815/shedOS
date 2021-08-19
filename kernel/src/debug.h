#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdint.h>

#define assert(x,s) \
        if (!(x)) { \
                kernel_panic(s, __FILE__, __LINE__); \
        }

void kernel_panic(const char *message, const char *file, int line);

#endif
