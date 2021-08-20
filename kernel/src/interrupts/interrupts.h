#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>
#include "isr.h"

struct registers {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
} __attribute__((packed));

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
extern void(*__isr32)(struct interrupt_frame*); /* timer */

#endif
