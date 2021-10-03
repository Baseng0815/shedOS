#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include "../task/task.h"

#include <stdint.h>

struct interrupt_frame_other {
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t ss;
} __attribute__((packed));

struct exception_frame {
        struct registers gprs;
        uint64_t int_no;
        uint64_t error_code;
        struct interrupt_frame_other frame;
} __attribute__((packed));

struct interrupt_frame {
        struct registers gprs;
        struct interrupt_frame_other frame;
} __attribute__((packed));

extern uintptr_t __exception_interrupts[30];
extern void(*__isr34)(struct interrupt_frame*); /* HPET */
extern void(*__isr128)(struct interrupt_frame*); /* syscall */
/* __attribute__((interrupt)) void __isr32(struct interrupt_frame *frame); */

#endif
