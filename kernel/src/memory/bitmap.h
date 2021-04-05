#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct bitmap {
        uint8_t *buf;
        size_t len; /* byte count */
};

bool bitmap_isset(struct bitmap*, size_t);
void bitmap_set(struct bitmap*, size_t);
void bitmap_unset(struct bitmap*, size_t);
void bitmap_flip(struct bitmap*, size_t);

#endif
