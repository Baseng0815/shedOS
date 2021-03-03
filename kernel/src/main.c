#include "framebuffer.h"
#include "font.h"
#include "terminal.h"

struct bootinfo {
        struct framebuffer  *framebuffer;
        struct psf1_font    *font;
};

static inline void ppx(int x, int y,
                       uint32_t pixel,
                       struct framebuffer *framebuffer)
{
        *((uint32_t*)(framebuffer->addr + framebuffer->pitch * y + 4 * x))
                = pixel;
}

void _start(struct bootinfo *bootinfo)
{
        framebuffer_initialize(bootinfo->framebuffer);
        font_initialize(bootinfo->font);

        /* number of characters in x/y direction */
        int cx = 80;
        int cy = 80;
        terminal_initialize(cx, cy);

        const char *a = "ASD";
        for (int i = 0; i < 10; i++) {
                /* for some reason, this doesn't work */
                terminal_puts("Hello World!");
        }

        while (true) {}
}
