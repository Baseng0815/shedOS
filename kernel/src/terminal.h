#ifndef _TERMINAL_H
#define _TERMINAL_H


#include <stdint.h>

void terminal_initialize(int, int);
void terminal_setcolor(uint32_t, uint32_t);
void terminal_putchar(char);
void terminal_puts(const char*);
void terminal_clear(void);

#endif
