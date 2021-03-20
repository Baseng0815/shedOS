#ifndef _PFA_H
#define _PFA_H

#include "efi_memory.h"

#define PF_BLOCKS_MAX 8

/* the page frame allocator is responsible for providing physical pages
   ("page frames") on request, which are later mapped to virtual addresses
   by the paging handler
   */

void pfa_initialize(struct efi_memory_map*);
void *pfa_request_page();
void pfa_release_page(void*);
size_t pfa_get_mem_total();
size_t pfa_get_mem_free();

#endif
