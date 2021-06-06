#include "kmalloc.h"

#include "memutil.h"
#include "../memory/vmm.h"

#include "printf.h"

struct mheap_hdr {
        size_t length; /* 64 byte aligned */
        struct mheap_hdr *next;
};

/* page heap starting 4G + 32K above the higher half */
static const uintptr_t pheap_start  = 0xffff800100000000;
static uintptr_t pnext_free         = pheap_start;

/* kernel heap starting at 16T above the higher half */
static const uintptr_t mheap_start  = 0xffff900000000000;
static struct mheap_hdr *mheap_head = (struct mheap_hdr*)mheap_start;

void kmalloc_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target kmalloc.\n");
        printf(KMSG_LOGLEVEL_INFO, "Kernel page heap at %a, mem heap at %a\n",
               pheap_start, mheap_start);

        memset(mheap_head, 0, sizeof(struct mheap_hdr));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target kmalloc.\n");
}

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
