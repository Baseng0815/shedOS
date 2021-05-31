#include "kmalloc.h"

#include "memutil.h"
#include "../memory/vmm.h"

#include "printf.h"

/* page heap starting 4G + 32K above the higher half */
static const uintptr_t pheap_start  = 0xffff800100000000;
static uintptr_t pnext_free         = pheap_start;

void *kpmalloc()
{
        void *memory = pnext_free;
        vmm_request_at(kernel_table, pnext_free, 1);

        /* find next free virtual address */
        struct pt_entry *entry;
        do {
                pnext_free++;
                entry = paging_entry_get(kernel_table, pnext_free);
        } while (entry->present);

        return memory;
}

void *kpzmalloc()
{
        void *memory = kpmalloc();
        memset(memory, 0, 0x1000);
        return memory;
}

void kpfree(void *memory)
{
        vmm_release_at(kernel_table, memory, 1);
        if (memory < pnext_free)
                pnext_free = memory;
}
