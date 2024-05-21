#ifndef _ALLOC_H
#define _ALLOC_H

#include <stddef.h>
#include <stdint.h>

/* actual not-so-shitty alloc */
void *malloc(size_t n, size_t alignment);
void mfree(void *addr);

/* page alloc */
void *palloc(size_t n);
void pfree(void*, size_t n);

void _malloc_init(void);

#endif
