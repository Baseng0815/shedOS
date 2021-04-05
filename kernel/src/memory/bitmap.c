#include "bitmap.h"

bool bitmap_isset(struct bitmap *bm, size_t n)
{
        return (bm->buf[n / 8] & 1 << (n % 8)) != 0;
}

void bitmap_set(struct bitmap *bm, size_t n)
{
        bm->buf[n / 8] |= 1 << (n % 8);
}

void bitmap_unset(struct bitmap *bm, size_t n)
{
        bm->buf[n / 8] &= ~(1 << (n % 8));
}

void bitmap_flip(struct bitmap *bm, size_t n)
{
        bm->buf[n / 8] ^= 1 << (n % 8);
}
