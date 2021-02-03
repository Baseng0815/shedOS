#ifndef __KERNEL_TTY_H
#define __KERNEL_TTY_H

/* freestanding headers */
#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
/* put string and scroll if necessary */
void terminal_putstring(const char *data);
/* scroll down one line */
void terminal_scroll();
void terminal_clear();

#endif
