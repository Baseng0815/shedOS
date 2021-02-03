/* freestanding headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t *VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void terminal_initialize(void)
{
        terminal_row = 0;
        terminal_column = 0;
        terminal_buffer = VGA_MEMORY;
        terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void terminal_setcolor(uint8_t color)
{
        terminal_color = color;
}

void terminal_putchar(char c)
{
        size_t index = terminal_row * VGA_WIDTH + terminal_column;
        if (c == '\n') {
                /* unset cursor */
                terminal_buffer[index] = vga_entry('\0', terminal_color);

                /* end of screen; scroll down */
                if (++terminal_row == VGA_HEIGHT) {
                        terminal_scroll();
                }
                terminal_column = 0;
        } else {

                terminal_buffer[index] = vga_entry(c, terminal_color);
                /* end of line; wrap around */
                if (++terminal_column == VGA_WIDTH) {
                        terminal_column = 0;
                        /* end of screen; scroll down */
                        if (++terminal_row == VGA_HEIGHT) {
                                terminal_scroll();
                        }
                }
        }

        /* set cursor (white box) of next char */
        if (terminal_column != VGA_WIDTH - 1) {
                uint8_t white = vga_entry_color(VGA_COLOR_WHITE,
                                                VGA_COLOR_WHITE);
                index = terminal_row * VGA_WIDTH + terminal_column;
                terminal_buffer[index] = vga_entry(' ', white);
        }
}

void terminal_putstring(const char *data)
{
        size_t size = strlen(data);
        for (size_t i = 0; i < size; i++) {
                terminal_putchar(data[i]);
        }
}

void terminal_scroll()
{
        for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                        size_t src = (y + 1) * VGA_WIDTH + x;
                        size_t dst = y * VGA_WIDTH + x;
                        terminal_buffer[src] = terminal_buffer[dst];
                }
        }
        terminal_row--;
}

void terminal_clear()
{
        uint8_t default_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        uint16_t clr = vga_entry('\0', default_color);
        for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
                terminal_buffer[i] = clr;
        }
        terminal_row = 0;
        terminal_column = 0;
}
