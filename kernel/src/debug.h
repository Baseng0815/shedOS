#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdint.h>

#define assert(x,s) \
        if (!(x)) { \
                kernel_panic(s, __FILE__, __LINE__); \
        }

struct register_dump {
        uint64_t rax, rbx, rcx, rdx;
        uint64_t rsi, rdi, rbp, rsp;
        uint64_t r8, r9, r10, r11;
        uint64_t r12, r13, r14, r15;
        uint64_t rip;
        uint16_t es, cs, ss, ds, fs, gs;
        uint16_t idt_limit;
        uint64_t idt_base;
        uint16_t gdt_limit;
        uint64_t gdt_base;
        uint64_t cr0, cr2, cr3, cr4;
        uint64_t dr0, dr1, dr2, dr3, dr6, dr7;
} __attribute__((packed));

void kernel_panic(const char *message, const char *file, int line);

extern void register_dump_make(struct register_dump**);

#endif
