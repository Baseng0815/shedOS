#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_PRINT 2
#define SYSCALL_FORK 57

#include <stdint.h>

struct interrupt_frame;

extern uint64_t (*syscalls[])(struct interrupt_frame *frame);

#endif
