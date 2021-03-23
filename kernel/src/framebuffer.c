#include "framebuffer.h"

struct framebuffer *fb;
int fb_width;
int fb_height;

void framebuffer_drawborder(uint32_t);

void framebuffer_initialize(struct framebuffer *_fb)
{
        fb          = _fb;
        fb_width    = fb->width     - 16;
        fb_height   = fb->height    - 16;

        framebuffer_drawborder(0x1c8aa6);
}

void framebuffer_putpixel(int x, int y, uint32_t px)
{
        x += 8;
        y += 8;
        *((uint32_t*)(fb->addr + fb->pitch * y + x * 4)) = px;
}

uint32_t framebuffer_getpixel(int x, int y)
{
        x += 8;
        y += 8;
        return *((uint32_t*)
                 (fb->addr + fb->pitch * y + x * 4));
}

void framebuffer_drawborder(uint32_t color)
{
        /* rows */
        for (int x = 0; x < (int)fb->width; x++) {
                for (int y = 0; y < 8; y++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 8, y - 16 + fb->height, color);
                }
        }

        /* cols */
        for (int y = 0; y < (int)fb->height; y++) {
                for (int x = 0; x < 8; x++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 16 + fb->width, y - 16, color);
                }
        }
}
