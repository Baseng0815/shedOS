#ifndef _VMM_H
#define _VMM_H

#include "paging.h"

void vmm_request_at(uint64_t *table, void *vaddr,
                    size_t n, uint8_t flags);
void vmm_release_at(uint64_t *table, void *vaddr, size_t n);

#endif
