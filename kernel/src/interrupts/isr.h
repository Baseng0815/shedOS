#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>


__attribute__((interrupt)) void hpet_handle(struct interrupt_frame*);

#endif
