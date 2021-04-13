#ifndef _ASSERT_H
#define _ASSERT_H

#include "libk/printf.h"

/* TODO panic screen with reg/mem dump*/
#define assert(x) \
        if (!(x)) { \
                printf(KMSG_LOGLEVEL_CRIT, \
                       "assert failed at %s:%d\n", \
                       __FILE__, __LINE__); \
                for (;;) asm("hlt"); \
        }

#endif
