#include "paging.h"

#include "pfa.h"
#include "../libk/memutil.h"
#include "../libk/printf.h"

static struct page_table *kernel_table;

/* get pdpi, pdi, pti, pi from vaddress */
static void map_index(uintptr_t, size_t*, size_t*, size_t*, size_t*);

/* get the next lower table or create if not existing */
static struct page_table *get(struct page_table*, size_t);

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
                child = (struct page_table*)pfa_request_page();
                memset(child, 0, 0x1000);

                child_entry->present = true;
                child_entry->writable = true;
                child_entry->addr = (uintptr_t)child >> 12;
        } else {
                /* child is present, all good */
                child = (struct page_table*)
                        ((uintptr_t)child_entry->addr << 12);
        }

        return child;
}

void paging_initialize(struct stivale2_struct_tag_memmap *mmap)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target paging.\n");

        kernel_table = (struct page_table*)pfa_request_page();
        memset(kernel_table, 0, 0x1000);

        printf(KMSG_LOGLEVEL_INFO,
               "Kernel identity table at %x\n", kernel_table);

        /* identity map the first 4 GiB and also high mem */
        printf(KMSG_LOGLEVEL_INFO,
               "Identity mapping first 4 GiB...\n");

        for (uintptr_t addr = 0x0; addr < 0x100000000; addr += 0x1000) {
                paging_map((void*)addr,
                           (void*)addr);
                paging_map((void*)(0x0 +                addr),
                           (void*)(0xffff800000000000 + addr));
        }

        /* identity map every mmap entry >= 4 GiB and also high mem */
        for (size_t i = 0; i < mmap->entries; i++) {
                struct stivale2_mmap_entry *entry = &mmap->memmap[i];
                if (entry->base < 0x100000000)
                        continue;

                printf(KMSG_LOGLEVEL_INFO,
                       "Identity mapping region %x-%x...\n",
                       entry->base, entry->base + entry->length);

                for (uintptr_t addr = entry->base;
                     addr < entry->base + entry->length;
                     addr += 0x1000) {
                        paging_map((void*)addr,
                                   (void*)addr);
                        paging_map((void*)(0x0                  + addr),
                                   (void*)(0xffff800000000000   + addr));
                }
        }

        /* map higher half kernel addresses to lower physical region */
        for (uintptr_t offset = 0x0; offset < 0x80000000; offset += 0x1000) {
                paging_map((void*)(0x0                  + offset),
                           (void*)(0xffffffff80000000   + offset));
        }

        paging_update();

        printf(KMSG_LOGLEVEL_OKAY, "Finished target paging.\n");
}

void paging_map(void *paddr,
                void *vaddr)
{
        size_t pml4i, pdpi, pdi, pti;
        map_index((uintptr_t)vaddr, &pml4i, &pdpi, &pdi, &pti);

        struct page_table *pdp      = get(kernel_table, pml4i);
        struct page_table *pd       = get(pdp, pdpi);
        struct page_table *pt       = get(pd, pdi);
        struct pt_entry *pt_entry   = &pt->entries[pti];
        /* set physical address */
        pt_entry->present = true;
        pt_entry->writable = true;
        pt_entry->addr = (uintptr_t)paddr >> 12;
}

void paging_update()
{
        asm volatile("mov %0, %%cr3"
                     :
                     : "r" (kernel_table));
}
