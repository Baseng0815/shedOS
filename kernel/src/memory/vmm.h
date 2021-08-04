#ifndef _VMM_H
#define _VMM_H

#include "paging.h"

void vmm_request_at(struct page_table *table, void *vaddr, size_t n,
                    bool cache_disabled, bool writable);
void vmm_release_at(struct page_table *table, void *vaddr, size_t n);

#endif
