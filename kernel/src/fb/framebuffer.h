#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stddef.h>
#include <stdint.h>

#include "../stivale2.h"

extern int fb_width;
extern int fb_height;

void framebuffer_initialize(struct stivale2_struct_tag_framebuffer*);
void framebuffer_putpixel(int, int, uint32_t);
uint32_t framebuffer_getpixel(int, int);

#endif
