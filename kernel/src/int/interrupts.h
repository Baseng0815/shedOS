#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

struct interrupt_frame;
__attribute__((interrupt)) void pf_handle(struct interrupt_frame*);

#endif
