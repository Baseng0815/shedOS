#include "syscalls.h"

#include "print.h"
#include "fork.h"

uint64_t (*syscalls[])(struct interrupt_frame *frame) = {
        [SYSCALL_PRINT] = syscall_print,
        [SYSCALL_FORK] = syscall_fork
};
