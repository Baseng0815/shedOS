#include <stddef.h>
#include <stdint.h>

struct framebuffer {
        void *addr;
        size_t size;
        uint32_t width;
        uint32_t height;
        uint32_t scanline_width;
        uint32_t pitch;
};

struct bootinfo {
        struct framebuffer *framebuffer;
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
        for (int x = 200; x < 400; x++)
                for (int y = 200; y < 400; y++)
                        ppx(x, y, 0x0fff0fff, bootinfo->framebuffer);
}
