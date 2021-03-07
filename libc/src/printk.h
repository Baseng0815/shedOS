#ifndef _PRINTK_H
#define _PRINTK_H

#include <stddef.h>
#include <stdint.h>

enum KMSG_URGENCY {
        KMSG_URGENCY_LOW, /* green (used for normal logging; green => haxx0r)*/
        KMSG_URGENCY_MEDIUM, /* orange (used for warnings) */
        KMSG_URGENCY_HIGH /* red (used for errors) */
};

void printk(int, const char*, ...);
void putchar(char);
void puts(const char*);

static size_t utos(uint64_t, uint64_t, char*);

#endif
