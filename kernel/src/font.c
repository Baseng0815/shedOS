#include "font.h"

void font_initialize(struct psf1_font *_font)
{
        font = _font;
}

bool font_is_pixel(char c, int x, int y)
{
        char *fontp = font->glyphs + (c * font->header->charsize) + y;
        return ((*fontp) & (0b10000000 >> x)) > 0;
}
