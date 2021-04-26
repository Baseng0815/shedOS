#ifndef _IRQS_H
#define _IRQS_H

struct interrupt_frame;

__attribute__((interrupt)) void hpet_handle(struct interrupt_frame*);

#endif
