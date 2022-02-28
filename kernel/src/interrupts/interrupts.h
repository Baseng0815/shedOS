#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

/* exceptions */
#define EXCEPTION_DE 0 /* divide error */
#define EXCEPTION_DB 1 /* debug */
#define EXCEPTION_BP 3 /* breakpoint */
#define EXCEPTION_OF 4 /* overflow */
#define EXCEPTION_BR 5 /* bound range exceeded */
#define EXCEPTION_UD 6 /* undefined opcode */
#define EXCEPTION_NM 7 /* device not available (math coprocessor) */
#define EXCEPTION_DF 8 /* double fault */
#define EXCEPTION_TS 10 /* invalid TSS */
#define EXCEPTION_NP 11 /* segment not present */
#define EXCEPTION_SS 12 /* stack segment fault */
#define EXCEPTION_GP 13 /* general protection fault */
#define EXCEPTION_PF 14 /* page fault */
#define EXCEPTION_MF 16 /* floating-point error (math fault) */
#define EXCEPTION_AC 17 /* alignment check */
#define EXCEPTION_MC 18 /* machine check */
#define EXCEPTION_XM 19 /* SIMD floating-point exception */
#define EXCEPTION_VE 20 /* virtualization exception */
#define EXCEPTION_CP 21 /* control protection */

#include "../task/task.h"

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
extern uint64_t(*__isr128)(struct interrupt_frame*); /* syscall */
/* __attribute__((interrupt)) void __isr32(struct interrupt_frame *frame); */

#endif
