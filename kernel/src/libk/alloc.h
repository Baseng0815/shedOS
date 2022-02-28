#ifndef _ALLOC_H
#define _ALLOC_H

#include <stddef.h>
#include <stdint.h>

/* bump alloc */
void *balloc(size_t, size_t);
void bfree(void);

/* page alloc */
void *palloc(size_t);
void pfree(void*, size_t);

#endif
