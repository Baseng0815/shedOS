#ifndef _IRQS_H
#define _IRQS_H

struct interrupt_frame;
__attribute__((interrupt)) void kb_handle(struct interrupt_frame*);

#endif
