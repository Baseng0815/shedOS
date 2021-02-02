#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler; you will most certainly run into trouble"
#endif

#if !defined(__i386)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

enum vga_color {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_MAGENTA = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_LIGHT_GREY = 7,
        VGA_COLOR_DARK_GREY = 8,
        VGA_COLOR_LIGHT_BLUE = 9,
        VGA_COLOR_LIGHT_GREEN = 10,
        VGA_COLOR_LIGHT_CYAN = 11,
        VGA_COLOR_LIGHT_RED = 12,
        VGA_COLOR_LIGHT_MAGENTA = 13,
        VGA_COLOR_LIGHT_BROWN = 14,
        VGA_COLOR_WHITE = 15,
};

size_t strlen(const char *str)
{
        size_t len = 0;
        while (str[len])
                len++;
        return len;
}

static const size_t VGA_WIDTH   = 80;
static const size_t VGA_HEIGHT  = 25;
size_t terminal_row;
size_t terminal_column;
uint16_t *terminal_buffer;

uint8_t terminal_vgacolor(enum vga_color fg,
                          enum vga_color bg)
{
        return fg | bg << 4;
}

void terminal_putchar(unsigned char c, uint8_t color)
{
        /* handle newline */
        if (c == '\n') {
                terminal_row++;
                terminal_column = 0;
                return;
        }

        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = (uint16_t)c | (uint16_t)color << 8;
        terminal_column++;
        /* wrap around */
        if (terminal_column >= VGA_WIDTH - 1) {
                terminal_column = 0;
                terminal_row++;
        }
}

void terminal_putstring(const char *str, uint8_t color)
{
        size_t slen = strlen(str);
        for (size_t i = 0; i < slen; i++) {
                terminal_putchar(str[i], color);
        }
}

void kernel_main(void)
{
        terminal_row = 0;
        terminal_column = 0;
        terminal_buffer = (uint16_t*)0xB8000;

        uint8_t color1 = terminal_vgacolor(VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
        uint8_t color2 = terminal_vgacolor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_RED);
        terminal_putstring("Hello World\n", color1);
        for (char c = 'a'; c <= 'z'; c++) {
                terminal_putchar(c, color2);
        }
}
