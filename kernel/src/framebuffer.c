#include "framebuffer.h"

void framebuffer_initialize(struct framebuffer *_framebuffer)
{
        framebuffer = _framebuffer;
}

void framebuffer_putpixel(int x, int y, uint32_t px)
{
        *((uint32_t*)(framebuffer->addr + framebuffer->pitch * y + x * 4)) = px;
}

uint32_t framebuffer_getpixel(int x, int y)
{
        return *((uint32_t*)
                 (framebuffer->addr + framebuffer->pitch * y + x * 4));
}
