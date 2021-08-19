#include "vmm.h"

#include "pmm.h"

#include "../libk/printf.h"

void vmm_request_at(struct page_table *table, void *vaddr,
                    size_t n, uint8_t flags)
{
        for (size_t i = 0; i < n; i++) {
                void *memory = pmm_request_pages(1);

                paging_map(table,
                           (uintptr_t)vaddr + (uintptr_t)(i * 0x1000),
                           memory, flags);
        }
}

void vmm_release_at(struct page_table *table, void *vaddr, size_t n)
{
        uint64_t *entry = paging_entry_get(kernel_table, vaddr);
        pmm_release_pages(*entry & ~0xfffUL, n);
        paging_unmap(kernel_table, vaddr);
}
