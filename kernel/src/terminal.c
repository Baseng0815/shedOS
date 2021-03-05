#include "terminal.h"

#include "font.h"
#include "framebuffer.h"

/* number of characters in x/y direction */
static int width, height;
static int chr_index;
static uint32_t fg, bg;

void terminal_initialize(int _width, int _height)
{
        width = _width;
        height = _height;

        chr_index = 0;
        fg = 0xffffffff;
        bg = 0x0;

        terminal_clear();
}

void terminal_setcolor(uint32_t _fg, uint32_t _bg)
{
        fg = _fg;
        bg = _bg;
}

void terminal_putchar(char c)
{
        // handle newline
        if (c == '\n') {
                chr_index += width - (chr_index + width) % width;
                return;
        }

        // handle other chars
        int ci_x = chr_index % width;
        int ci_y = chr_index / width;
        for (int ix = 0; ix < 8; ix++) {
                for (int iy = 0; iy < 16; iy++) {
                        int x = ci_x * 8 + ix;
                        int y = ci_y * 16 + iy;
                        framebuffer_putpixel(x, y, font_is_pixel(c, ix, iy)
                                             ? fg
                                             : bg);
                }
        }

        chr_index++;
}

void terminal_puts(const char *str)
{
        if (str == NULL)
                return;

        while (*str != '\0') {
                terminal_putchar(*(str++));
        }
}

void terminal_clear(void)
{
        for (int x = 0; x < 8 * width; x++) {
                for (int y = 0; y < 16 * height; y++) {
                        framebuffer_putpixel(x, y, 0x0);
                }
        }

        chr_index = 0;
}
