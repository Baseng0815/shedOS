#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

/* kernel timer uses microseconds */
extern uint64_t timer_elapsed_us;

void timer_initialize();

void timer_tick(uint64_t dus);
char *timer_format(char*);

#endif
