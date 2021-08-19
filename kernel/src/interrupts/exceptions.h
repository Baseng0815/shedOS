#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include <stdint.h>
#include "isr.h"

struct exception_frame {
        struct registers gprs;
        uint64_t int_no;
        uint64_t error_code;
        struct interrupt_frame frame;
} __attribute__((packed));

extern void exception_handle(struct exception_frame*);

/* exception handlers */
extern void(*exception_interrupts[])(struct exception_frame*);
extern const char *exception_names[];

#endif
