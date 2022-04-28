#include "sched.h"

#include "../interrupts/timer.h"
#include "../libk/printf.h"

struct task *current_task;

void sched_run(struct task *initial_task)
{
        current_task = initial_task;
        uint64_t current_time = timer_read_counter();
        current_task->deadline = current_time + 1000000;

        printf(KMSG_LOGLEVEL_WARN, "first: switch to %d\n", current_task->id);
        switch_to_task(initial_task);
}

void sched_tick(const struct interrupt_frame *ifr)
{
        uint64_t current_time = timer_read_counter();

        if (current_time > current_task->deadline) {
                /* save context */
                current_task->regs      = ifr->gprs;
                current_task->rflags    = ifr->frame.rflags;
                current_task->rip       = ifr->frame.rip;
                current_task->rsp       = ifr->frame.rsp;

                /* reset deadline and switch to new task */
                current_task = current_task->next_task;
                current_task->deadline = current_time + 1000000;
                printf(KMSG_LOGLEVEL_WARN, "old rip: %x\n", ifr->frame.rip);
                printf(KMSG_LOGLEVEL_WARN, "switch to %d\n", current_task->id);
                switch_to_task(current_task);
        }
}
