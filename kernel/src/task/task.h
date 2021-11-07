#ifndef _TASK_H
#define _TASK_H

#include "../memory/paging.h"

#include <stdint.h>

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

struct task {
        struct registers regs;
        uint64_t rsp;
        uint64_t rflags;
        uint64_t rip;
        struct task *next_task;
        uint64_t *vmap; /* virtual memory map */
        uint64_t deadline;
} __attribute__((packed));

/* SysV ABI: RDI, RSI, RDX, RCX, R8, R9 */

/* create task from elf data */
void task_create(struct task **new_task, const uint8_t *elf_data);

/* load task context and transfer control */
void switch_to_task(const struct task *task);

#endif
