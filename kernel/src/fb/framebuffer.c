#include "framebuffer.h"

int fb_width;
int fb_height;
static struct stivale2_struct_tag_framebuffer *fb;

static void framebuffer_drawborder(uint32_t);

void framebuffer_initialize(struct stivale2_struct_tag_framebuffer *_fb)
{
        fb = _fb;
        fb_width    = fb->framebuffer_width;
        fb_height   = fb->framebuffer_height;

        framebuffer_drawborder(0x1c8aa6);
}

void framebuffer_putpixel(int x, int y, uint32_t px)
{
        x += 8;
        y += 8;
        /* TODO use framebuffer masks instead of non-based assumptions */
        *(uint32_t*)(fb->framebuffer_addr
                + y * fb->framebuffer_pitch
                + x * fb->framebuffer_bpp / 8) = px;
}

uint32_t framebuffer_getpixel(int x, int y)
{
        x += 8;
        y += 8;
        return *(uint32_t*)(fb->framebuffer_addr
                            + y * fb->framebuffer_pitch
                            + x * fb->framebuffer_bpp / 8);
}

void framebuffer_drawborder(uint32_t color)
{
        /* rows */
        for (int x = 0; x < (int)fb->framebuffer_width; x++) {
                for (int y = 0; y < 8; y++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 8,
                                             y - 16 + fb->framebuffer_height,
                                             color);
                }
        }

        /* cols */
        for (int y = 0; y < (int)fb->framebuffer_height; y++) {
                for (int x = 0; x < 8; x++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 16 + fb->framebuffer_width,
                                             y - 16,
                                             color);
                }
        }
}
