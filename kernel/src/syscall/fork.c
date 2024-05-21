#include "fork.h"

#include "../interrupts/interrupts.h"

#include "../libk/alloc.h"
#include "../libk/memutil.h"
#include "../libk/printf.h"

#include <stdarg.h>
#include "../task/sched.h"

#include "../terminal/terminal.h"

uint64_t syscall_fork(struct interrupt_frame *frame)
{
        struct task *new_task = malloc(sizeof(struct task), 0);
        /* copy context from current process, including a fresh vmap */
        new_task->vmap      = paging_copy(current_task->vmap);
        new_task->regs      = frame->gprs;
        new_task->rflags    = frame->frame.rflags;
        new_task->rip       = frame->frame.rip;
        new_task->rsp       = frame->frame.rsp;
        new_task->id        = task_new_tid();

        /* le epic cow */
        paging_make_readonly(current_task->vmap);
        paging_make_readonly(new_task->vmap);

        new_task->next_task = current_task->next_task;
        current_task->next_task = new_task;

        new_task->regs.rax = 0;

        return 2;
}
