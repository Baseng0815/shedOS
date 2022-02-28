#include "print.h"

#include "../interrupts/interrupts.h"

#include "../libk/printf.h"

#include "../task/sched.h"

uint64_t syscall_print(struct interrupt_frame *frame)
{
        printf(KMSG_LOGLEVEL_WARN, (const char*)frame->gprs.rdi);

        return 0;
}
