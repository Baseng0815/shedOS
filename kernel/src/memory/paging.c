#include "paging.h"

#include "pmm.h"
#include "../libk/memutil.h"
#include "../libk/printf.h"

struct page_table *kernel_table;

/* get pdpi, pdi, pti, pi from vaddress */
static void map_index(uintptr_t, size_t*, size_t*, size_t*, size_t*);

/* get the next lower table or create if not existing */
static struct page_table *get(struct page_table*, size_t);

static void map_kernel_region(uintptr_t, uintptr_t, size_t);

void paging_initialize(struct stivale2_struct_tag_memmap *mmap,
                       struct stivale2_struct_tag_framebuffer *fb)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target paging.\n");

        kernel_table = (struct page_table*)pmm_request_pages(1);
        memset(kernel_table, 0, 0x1000);
        paging_map(kernel_table, (void*)kernel_table, (void*)kernel_table);

        printf(KMSG_LOGLEVEL_INFO,
               "Kernel table at %a\n", kernel_table);

        /* map everything below 4G to higher half */
        printf(KMSG_LOGLEVEL_INFO, "Mapping first 4G...\n");
        map_kernel_region(VADDR_HIGHER, 0x0, 0x100000000);
        map_kernel_region(VADDR_KERNEL, 0x0, 0x100000000);

        printf(KMSG_LOGLEVEL_INFO, "Using new page table...\n");

        paging_update(kernel_table);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target paging.\n");
}

void paging_map(struct page_table *table,
                void *vaddr,
                void *paddr)
{
        size_t pml4i, pdpi, pdi, pti;
        map_index((uintptr_t)vaddr, &pml4i, &pdpi, &pdi, &pti);

        struct page_table *pdp      = get(table, pml4i);
        struct page_table *pd       = get(pdp, pdpi);
        struct page_table *pt       = get(pd, pdi);
        struct pt_entry *pt_entry   = &pt->entries[pti];
        /* set physical address */
        pt_entry->present = true;
        pt_entry->writable = true;
        pt_entry->addr = (uintptr_t)paddr >> 12;
}

void paging_update(const struct page_table *table)
{
        asm volatile("mov %0, %%cr3"
                     :
                     : "r" (table));
}

void map_index(uintptr_t vaddr,
               size_t *pml4i,
               size_t *pdpi,
               size_t *pdi,
               size_t *pti)
{
        /* 48 bits of the virtual address are translated,
           so we have access to 256TB
           the first 12 bit are used for indexing inside the page
           each consecutive 9-bit block is used to index into a page table
           */

        vaddr >>= 12;
        *pti = vaddr & 0x1ff; /* 0x1ff = 9-bit mask */
        vaddr >>= 9;
        *pdi = vaddr & 0x1ff;
        vaddr >>= 9;
        *pdpi = vaddr & 0x1ff;
        vaddr >>= 9;
        *pml4i = vaddr & 0x1ff;
}

struct page_table *get(struct page_table *parent,
                       size_t index)
{
        struct pt_entry *child_entry = &parent->entries[index];
        struct page_table *child;
        if (!child_entry->present) {
                /* allocate and zero out */
                child = (struct page_table*)pmm_request_pages(1);
                memset(child, 0, 0x1000);

                child_entry->present = true;
                child_entry->writable = true;
                child_entry->addr = (uintptr_t)child >> 12;

                paging_map(kernel_table, (void*)child, (void*)child);
        } else {
                /* child is present, all good */
                child = (struct page_table*)
                        ((uintptr_t)child_entry->addr << 12);
        }

        return child;
}

static void map_kernel_region(uintptr_t voffset,
                              uintptr_t poffset,
                              uintptr_t len)
{
        for (uintptr_t addr = poffset;
             addr < poffset + len;
             addr += 0x1000) {
                paging_map(kernel_table, (void*)(voffset + addr), (void*)addr);
        }
}
