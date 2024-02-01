#ifndef _TIMER_H
#define _TIMER_H

#include "interrupts.h"

#include <stdint.h>

void timer_initialize(void);

void timer_tick(const struct interrupt_frame *ifr, int dus);
uint64_t timer_read_counter(void);

char *timer_format(char*);

#endif
