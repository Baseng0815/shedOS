#ifndef _ASSERT_H
#define _ASSERT_H

#include "libk/printf.h"

#define assert(x,s) \
        if (!(x)) { \
                printf(KMSG_LOGLEVEL_CRIT, \
                       "assert failed at %s:%d: %s\n", \
                       __FILE__, __LINE__, s); \
                for (;;) asm("hlt"); \
        }

#endif
