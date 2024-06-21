#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_PRINT 2
#define SYSCALL_FORK 57

#define SYSCALL_ENUMERATE_DEVICE_FUNCTIONS 4
#define SYSCALL_CALL_DEVICE_FUNCTION 5 /* interact with a device */
#define SYSCALL_CALL_KERNEL_FUNCTION 6 /* interact with a device */

#include <stdint.h>

struct interrupt_frame;

extern uint64_t (*syscalls[])(struct interrupt_frame *frame);

#endif
