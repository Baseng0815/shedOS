#ifndef _TASK_H
#define _TAKS_H

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
        struct task *next_task;
        uint64_t rsp;
        struct page_table *vmap; /* virtual memory map */
};

/* create task from elf executable file */
void create_task(struct task **new_task, uint8_t *elf_data);

#endif
