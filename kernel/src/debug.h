#ifndef _DEBUG_H
#define _DEBUG_H

#include "libk/printf.h"

#define assert(x,s) \
        if (!(x)) { \
                kernel_panic(s); \
        }

#define kernel_panic(s) \
        printf(KMSG_LOGLEVEL_CRIT, \
               "KERNEL PANIC: %s at %s:%d\n", \
               s, __FILE__, __LINE__); \
        for (;;) { \
                asm volatile("hlt"); \
        } \

#endif
