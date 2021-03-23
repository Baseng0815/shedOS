#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

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

extern int fb_width;
extern int fb_height;

void framebuffer_initialize(struct framebuffer*);
void framebuffer_putpixel(int, int, uint32_t);
uint32_t framebuffer_getpixel(int, int);

#endif
