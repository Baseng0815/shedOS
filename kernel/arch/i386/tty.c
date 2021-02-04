/* freestanding headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static const size_t LAST_VALID_INDEX = VGA_WIDTH * VGA_HEIGHT - 1;
static uint16_t *VGA_MEMORY = (uint16_t*)0xB8000;

/* where to insert next char in memory */
static size_t terminal_index;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void terminal_initialize(void)
{
        terminal_index = 0;
        terminal_buffer = VGA_MEMORY;
        terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void terminal_setcolor(uint8_t color)
{
        terminal_color = color;
}

void terminal_putchar(char c)
{
        if (c == '\n') {
                /* remove cursor */
                uint8_t empty = vga_entry('\0', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                terminal_buffer[terminal_index] = empty;
                terminal_index += VGA_WIDTH - terminal_index % VGA_WIDTH;
        } else {
                terminal_buffer[terminal_index++] = vga_entry(c, terminal_color);
                if (terminal_index > LAST_VALID_INDEX) {
                        terminal_scroll();
                }
        }

        /* set cursor */
        uint8_t white = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_WHITE);
        terminal_buffer[terminal_index] = vga_entry(' ', white);
}

void terminal_putstring(const char *data, size_t size)
{
        for (size_t i = 0; i < size; i++) {
                terminal_putchar(data[i]);
        }
}

void terminal_scroll()
{
        /* remove cursor */
        uint8_t empty = vga_entry('\0', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
        terminal_buffer[terminal_index] = empty;

        /* copy from line down below */
        for (size_t index = 0; index <= LAST_VALID_INDEX - VGA_WIDTH; index++) {
                terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
        }

        /* set index to beginning of current line (not on next line!) */
        terminal_index -= terminal_index % VGA_WIDTH;

        /* set cursor */
        uint8_t white = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_WHITE);
        terminal_buffer[terminal_index] = vga_entry(' ', white);
}
