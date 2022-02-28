#include "vmm.h"

#include "pmm.h"

#include "../libk/printf.h"

void vmm_request_at(uint64_t *table, void *vaddr,
                    size_t n, uint8_t flags)
{
        for (size_t i = 0; i < n; i++) {
                uint64_t target_vaddr = (uint64_t)vaddr + i * 0x1000;

                /* make sure vaddr is free */
                uint64_t entry = paging_get(table, (void*)target_vaddr);
                if (entry & PAGING_PRESENT)
                        continue;

                void *memory = pmm_request_pages(1);

                paging_map(table, (void*)target_vaddr, memory, flags);
        }
}

void vmm_release_at(uint64_t *table, void *vaddr, size_t n)
{
        uint64_t entry = paging_get(table, vaddr);
        /* is not present */
        if (!(entry & PAGING_PRESENT))
                return;

        pmm_release_pages((void*)(entry & ~0xfffUL), n);
        paging_unmap(table, vaddr);
}
