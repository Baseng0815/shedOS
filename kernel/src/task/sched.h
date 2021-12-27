#ifndef _SCHED_H
#define _SCHED_h

#include "task.h"
#include "../interrupts/interrupts.h"

extern struct task *current_task;

void sched_run(struct task *initial_task);
void sched_tick(const struct interrupt_frame *ifr);

#endif
