#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <stddef.h>

void malloc_initialize();

/* allocate a page */
void *kpmalloc(size_t count);

/* allocate a zero page */
void *kpzmalloc(size_t count);

#endif
