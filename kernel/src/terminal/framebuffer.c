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

static void framebuffer_drawborder(uint32_t);

void framebuffer_initialize(struct stivale2_struct_tag_framebuffer *fb)
{
        /* we assume 32bpp */
        fb_width    = fb->framebuffer_width;
        fb_height   = fb->framebuffer_height;
        fb_pitch    = fb->framebuffer_pitch;
        fb_buffer   = bump_alloc(fb_height * fb_pitch, 0);
        fb_mmio     = (uint32_t*)fb->framebuffer_addr;

        framebuffer_drawborder(0x1c8aa6);
}

void framebuffer_putpixel(int x, int y, uint32_t px)
{
        x += 8;
        y += 8;

        /* TODO use framebuffer masks instead of unbased assumptions */
        fb_buffer[y * fb_pitch / 4 + x] = px;
        fb_mmio[y * fb_pitch / 4 + x]   = px;
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
}
