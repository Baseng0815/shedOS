#ifndef _PFA_H
#define _PFA_H

#include <stddef.h>

#include "../stivale2.h"

/* the page frame allocator is responsible for providing physical pages
   ("page frames") on request, which are later mapped to virtual addresses
   by the vmm (virtual memory manager)
   */

void pfa_initialize(struct stivale2_struct_tag_memmap*);
void *pfa_request_page();
void pfa_release_page(void*);
size_t pfa_get_mem_total();
size_t pfa_get_mem_free();

#endif
