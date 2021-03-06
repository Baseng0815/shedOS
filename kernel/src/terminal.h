#ifndef _TERMINAL_H
#define _TERMINAL_H


#include <stdint.h>

/* number of characters in x/y direction */
static int width, height;
static int chr_index;
static uint32_t fg, bg;

static int width, height;
static int chr_index;
static uint32_t fg, bg;

void terminal_initialize(int, int);
void terminal_setcolor(uint32_t, uint32_t);
void terminal_putchar(char);
void terminal_puts(const char*);
void terminal_clear(void);

static void terminal_scroll(void);

#endif
