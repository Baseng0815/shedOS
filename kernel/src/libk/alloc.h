#ifndef _ALLOC_H
#define _ALLOC_H

#include <stddef.h>
#include <stdint.h>

/* bump alloc */
void *balloc(size_t n, size_t alignment);
void bfree(void);

/* page alloc */
void *palloc(size_t n);
void pfree(void*, size_t n);

#endif
