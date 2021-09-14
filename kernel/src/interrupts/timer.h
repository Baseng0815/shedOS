#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

extern const uint64_t ticks_per_second;

extern uint64_t ticks;

void timer_initialize();

/* this will be called by the hpet interrupt handler */
void timer_tick();
char *timer_format(char*);

#endif
