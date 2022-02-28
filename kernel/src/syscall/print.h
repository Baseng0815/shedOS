#ifndef _PRINT_H
#define _PRINT_H

#include <stdint.h>

struct interrupt_frame;

uint64_t syscall_print(struct interrupt_frame *frame);

#endif
