#include "framebuffer.h"

#include "../libk/memutil.h"
#include "../libk/bump_alloc.h"

#include "../memory/paging.h"
#include "../memory/addrutil.h"

int                         fb_width;
int                         fb_height;
static uint16_t             fb_pitch;
static uint32_t             *fb_buffer;
static volatile uint32_t    *fb_mmio;

static int                  redraw_y_begin, redraw_y_end;

static void framebuffer_drawborder(uint32_t);

void framebuffer_initialize(struct stivale2_struct_tag_framebuffer *fb)
{
        /* we assume 32bpp */
        fb_width    = fb->framebuffer_width;
        fb_height   = fb->framebuffer_height;
        fb_pitch    = fb->framebuffer_pitch;
        fb_buffer   = bump_alloc(fb_height * fb_pitch, 0);
        fb_mmio     = (uint32_t*)fb->framebuffer_addr;

        redraw_y_begin  = 0;
        redraw_y_end    = 0;

        framebuffer_drawborder(0x1c8aa6);
}

void framebuffer_putpixel(int x, int y, uint32_t px)
{
        x += 8;
        y += 8;
        if (y < redraw_y_begin) {
                redraw_y_begin = y;
        } else if (y > redraw_y_end) {
                redraw_y_end = y;
        }

        /* TODO use framebuffer masks instead of unbased assumptions */
        fb_buffer[y * fb_pitch / 4 + x] = px;
}

uint32_t framebuffer_getpixel(int x, int y)
{
        x += 8;
        y += 8;
        return fb_buffer[y * fb_pitch / 4 + x];
}

void framebuffer_drawborder(uint32_t color)
{
        /* rows */
        for (int x = 0; x < fb_width; x++) {
                for (int y = 0; y < 8; y++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 8,
                                             y - 16 + fb_height,
                                             color);
                }
        }

        /* cols */
        for (int y = 0; y < fb_height; y++) {
                for (int x = 0; x < 8; x++) {
                        framebuffer_putpixel(x - 8, y - 8, color);
                        framebuffer_putpixel(x - 16 + fb_width,
                                             y - 8,
                                             color);
                }
        }

        framebuffer_flush();
}

void framebuffer_flush(void)
{
        uint32_t *begin = (uintptr_t)fb_buffer + redraw_y_begin * fb_pitch;
        size_t size = (redraw_y_end - redraw_y_begin) * fb_pitch;
        memcpy_128(fb_mmio, begin, size);
        redraw_y_begin = 0;
        redraw_y_end = 0;
}
