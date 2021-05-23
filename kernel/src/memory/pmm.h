#ifndef _PMM_H
#define _PMM_H

#include <stddef.h>

#include "../stivale2.h"

/* the physical memory manager is responsible for providing physical pages
   ("page frames") on request, which are later mapped to virtual addresses
   by the vmm (virtual memory manager)
   */

extern size_t free_memory;
extern size_t total_memory;

void pmm_initialize(struct stivale2_struct_tag_memmap*);
void *pmm_request_pages(size_t);
void pmm_release_pages(void*, size_t);

#endif
