#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

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

struct exception_frame {
        struct registers gprs;
        uint64_t int_no;
        uint64_t error_code;
        struct interrupt_frame frame;
} __attribute__((packed));

extern void exception_handle(struct exception_frame*);

/* exception handlers as defined in exceptions.S */
extern uintptr_t __exception_interrupts[30];
extern const char *exception_names[];

#endif
