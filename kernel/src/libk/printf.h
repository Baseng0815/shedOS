#ifndef _PRINTF_H
#define _PRINTF_H

#include <stddef.h>
#include <stdint.h>

enum KMSG_LOGLEVEL {
        KMSG_LOGLEVEL_INFO,
        KMSG_LOGLEVEL_SUCC,
        KMSG_LOGLEVEL_WARN,
        KMSG_LOGLEVEL_CRIT,

        KMSG_LOGLEVEL_NONE
};

void printf(int, const char*, ...);
void putchar(char);
void puts(const char*);

#endif
