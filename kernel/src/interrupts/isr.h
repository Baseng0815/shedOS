#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

struct interrupt_frame {
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t ss;
} __attribute__((packed));

__attribute__((interrupt)) void hpet_handle(struct interrupt_frame*);

#endif
