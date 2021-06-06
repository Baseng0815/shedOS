#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <stddef.h>

void kmalloc_initialize();

/* allocate a page */
void *kpmalloc();

/* allocate a zero page */
void *kpzmalloc();

/* free a page */
void kpfree(void *memory);

#endif
