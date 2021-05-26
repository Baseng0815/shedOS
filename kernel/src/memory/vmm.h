#ifndef _VMM_H
#define _VMM_H

#include <stddef.h>
#include <stdint.h>

#include "paging.h"

void vmm_request_at(struct page_table *table, void *vaddr, size_t n);
void vmm_release_at(struct page_table *table, void *vaddr, size_t n);

#endif
