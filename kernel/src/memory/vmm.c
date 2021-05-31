#include "vmm.h"

#include "pmm.h"

#include "../libk/printf.h"

void vmm_request_at(struct page_table *table, void *vaddr, size_t n)
{
        for (size_t i = 0; i < n; i++) {
                void *memory = pmm_request_pages(1);

                paging_map(table,
                           (uintptr_t)vaddr + (uintptr_t)(i * 0x1000),
                           memory);
        }
}

void vmm_release_at(struct page_table *table, void *vaddr, size_t n)
{
        struct pt_entry *pt_entry = paging_entry_get(table, vaddr);
        pt_entry->present = false;

        pmm_release_pages((uintptr_t)pt_entry->addr << 12, n);
}
