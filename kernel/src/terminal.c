#include "terminal.h"

#include "font.h"
#include "framebuffer.h"

#include "libk/string.h"

void terminal_initialize(int _cx, int _cy)
{
        chr_index = 0;
        color = 0xffffffff;

        cx = _cx;
        cy = _cy;

        terminal_clear();
}

void terminal_setcolor(uint32_t _color)
{
        color = _color;
}

void terminal_putchar(char c)
{
        if (c == '\n') {
                // handle newline
                return;
        }

        // handle other chars
        int ci_x = chr_index % cy;
        int ci_y = chr_index / cy;
        for (int ix = 0; ix < 8; ix++) {
                for (int iy = 0; iy < 16; iy++) {
                        int x = ci_x * 8 + ix;
                        int y = ci_y * 16 + iy;
                        framebuffer_putpixel(x, y, font_is_pixel(c, ix, iy)
                                             ? color
                                             : 0x0);
                }
        }

        chr_index++;
}

void terminal_puts(const char *str)
{
        for (int i = 0; i < 10; i++) {
                terminal_putchar('a');
        }
}

void terminal_clear(void)
{
        for (int x = 0; x < 8 * cx; x++) {
                for (int y = 0; y < 16 * cy; y++) {
                        framebuffer_putpixel(x, y, 0x0);
                }
        }

        chr_index = 0;
}
