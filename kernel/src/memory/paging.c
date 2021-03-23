#include "paging.h"

#include "pfa.h"
#include <string.h>

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

struct page_table *paging_identity(struct framebuffer *fb)
{
        struct page_table *pml4 = (struct page_table*)pfa_request_page();
        memset(pml4, 0, 0x1000);

        /* map normal memory */
        for (uintptr_t addr = 0; addr < pfa_get_mem_total(); addr += 0x1000) {
                paging_map(pml4, (void*)addr, (void*)addr);
        }

        /* map framebuffer memory */
        for (uintptr_t addr = fb->addr;
             addr < (uintptr_t)fb->addr + fb->size + 0x1000;
             addr += 0x1000) {
                paging_map(pml4, (void*)addr, (void*)addr);
        }

        paging_use(pml4);

        return pml4;
}

void paging_map(struct page_table *pml4,
                void *paddr,
                void *vaddr)
{
        size_t pml4i, pdpi, pdi, pti;
        map_index((uintptr_t)vaddr, &pml4i, &pdpi, &pdi, &pti);

        struct page_table *pdp      = get(pml4, pml4i);
        struct page_table *pd       = get(pdp, pdpi);
        struct page_table *pt       = get(pd, pdi);
        struct pt_entry *pt_entry   = &pt->entries[pti];
        /* set physical address */
        pt_entry->present = true;
        pt_entry->writable = true;
        pt_entry->addr = (uintptr_t)paddr >> 12;
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

void paging_use(struct page_table *pml4)
{
        asm volatile("mov %0, %%cr3"
                     :
                     : "r" (pml4));
}
