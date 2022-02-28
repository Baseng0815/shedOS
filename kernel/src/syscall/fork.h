#ifndef _FORK_H
#define _FORK_H

#include <stdint.h>

struct interrupt_frame;

uint64_t syscall_fork(struct interrupt_frame *frame);

#endif
