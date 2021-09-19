#ifndef _BUMP_ALLOC_H
#define _BUMP_ALLOC_H

#include <stddef.h>
#include <stdint.h>

/* a bump allocator works basically like a stack with element alignment, meaning
 * we can only free in LIFO order or free the whole pool at once */
struct bump_alloc {
        uintptr_t bump_ptr;
        uintptr_t start;
        uintptr_t end;
};

void *bump_alloc(size_t n, size_t alignment);
void bump_free_all();

#endif
