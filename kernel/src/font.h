#ifndef _FONT_H
#define _FONT_H

#include <stdbool.h>

struct psf1_header {
        unsigned char magic[2];
        unsigned char mode;
        unsigned char charsize;
};

struct psf1_font {
        struct psf1_header  *header;
        void                *glyphs;
};

static struct psf1_font *font;
void font_initialize(struct psf1_font*);
/* returns true if font pixel is foreground */
bool font_is_pixel(char, int, int);

#endif
