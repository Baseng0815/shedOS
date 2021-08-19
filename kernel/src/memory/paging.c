#include "paging.h"

#include "pmm.h"
#include "../libk/memutil.h"
#include "../libk/printf.h"

#include "addrutil.h"

struct page_table *kernel_table;

/* get level indices */
static void map_index(uintptr_t, size_t*, size_t*, size_t*, size_t*);

/* get the next lower table or create if not existing */
static struct page_table *get(struct page_table*, size_t);

static void map_kernel_region(uintptr_t, uintptr_t, size_t);

void paging_initialize(struct stivale2_struct_tag_memmap *mmap,
                       struct stivale2_struct_tag_framebuffer *fb)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target paging.\n");

        /* disable kernel write access to read-only pages */
        asm volatile("movq %%cr0, %%rax;"
                     "orq $(1 << 16), %%rax;"
                     "movq %%rax, %%cr0;"
                     : : : "rax");

        kernel_table = (struct page_table*)
                vaddr_ensure_higher(pmm_request_pages(1));
        memset(kernel_table, 0, 0x1000);

        printf(KMSG_LOGLEVEL_INFO,
               "Kernel table at %a\n", kernel_table);

        /* map everything below 4G to higher half */
        printf(KMSG_LOGLEVEL_INFO, "Mapping first 4G to HIGHER...\n");
        map_kernel_region(VADDR_HIGHER, 0x0, 0x100000000);
        printf(KMSG_LOGLEVEL_INFO, "Mapping first 4G to KERNEL...\n");
        map_kernel_region(VADDR_KERNEL, 0x0, 0x100000000);

        /* unmap NULL page */
        paging_unmap(kernel_table, NULL);

        printf(KMSG_LOGLEVEL_INFO, "Using new page table...\n");

        paging_write_cr3(vaddr_ensure_lower(kernel_table));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target paging.\n");
}

bool paging_map(struct page_table *table,
                void *vaddr,
                void *paddr,
                uint8_t flags)
{
        uint64_t *entry = paging_entry_get(table, vaddr);

        /* already in use */
        if (*entry & PAGING_PRESENT)
                return false;

        *entry = PAGING_PRESENT | PAGING_USER |
                flags |
                (uintptr_t)paddr;

        paging_flush_tlb(vaddr);

        return true;
}

void paging_unmap(struct page_table *table, void *vaddr)
{
        uint64_t *entry = paging_entry_get(table, vaddr);

        *entry &= ~PAGING_PRESENT;
}

uint64_t *paging_entry_get(struct page_table *table, void *vaddr)
{
        size_t lvl4_index, lvl3_index, lvl2_index, lvl1_index;
        map_index((uintptr_t)vaddr,
                  &lvl4_index,
                  &lvl3_index,
                  &lvl2_index,
                  &lvl1_index);

        struct page_table *lvl4 = get(table, lvl4_index);
        struct page_table *lvl3 = get(lvl4, lvl3_index);
        struct page_table *lvl2 = get(lvl3, lvl2_index);
        uint64_t *lvl1  = &lvl2->entries[lvl1_index];

        return lvl1;
}

void paging_write_cr3(const struct page_table *table)
{
        asm volatile("movq %0, %%cr3" : : "r" (table));
}

void paging_flush_tlb(void *addr)
{
        asm volatile("invlpg (%0)" : : "r" (addr));
}

void map_index(uintptr_t vaddr,
               size_t *lvl4_index,
               size_t *lvl3_index,
               size_t *lvl2_index,
               size_t *lvl1_index)
{
        /* 48 bits of the virtual address are translated,
           so we have access to 256TB
           the first 12 bit are used for indexing inside the page
           each consecutive 9-bit block is used to index into a page table
           */

        vaddr >>= 12;
        *lvl1_index = vaddr & 0x1ff; /* 0x1ff = 9-bit mask */
        vaddr >>= 9;
        *lvl2_index = vaddr & 0x1ff;
        vaddr >>= 9;
        *lvl3_index = vaddr & 0x1ff;
        vaddr >>= 9;
        *lvl4_index = vaddr & 0x1ff;
}

struct page_table *get(struct page_table *parent, size_t index)
{
        uint64_t *child_entry = &parent->entries[index];
        struct page_table *child;
        if (!(*child_entry & PAGING_PRESENT)) {
                /* allocate and zero out */
                child = (struct page_table*)
                        vaddr_ensure_higher(pmm_request_pages(1));
                memset(child, 0, 0x1000);

                *child_entry = PAGING_PRESENT | PAGING_USER |
                        PAGING_WRITABLE |
                        vaddr_ensure_lower(child);

                paging_map(kernel_table,
                           (void*)child,
                           (void*)vaddr_ensure_lower(child), PAGING_WRITABLE);
        } else {
                /* child is present, all good (mask off first 12 bits) */
                child = (struct page_table*)
                        vaddr_ensure_higher(*child_entry & ~0xfffUL);
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
                paging_map(kernel_table, (void*)(voffset + addr), (void*)addr,
                           PAGING_WRITABLE);
        }
}
