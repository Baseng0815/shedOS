#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stdint.h>

static int chr_index;
static uint32_t color;
/* number of characters in x/y direction */
static int cx, cy;
void terminal_initialize(int, int);
void terminal_setcolor(uint32_t);
void terminal_putchar(char);
void terminal_puts(const char*);
void terminal_clear(void);

#endif
