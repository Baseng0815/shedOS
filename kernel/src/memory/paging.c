#include "paging.h"

#include "pmm.h"
#include "../libk/memutil.h"

#include "addrutil.h"

uint64_t *kernel_table;

/* get level indices */
static void map_index(uintptr_t, size_t*, size_t*, size_t*, size_t*);

static uint64_t *get(uint64_t*, size_t, bool);
static void _paging_copy_table(uint64_t*, uint64_t*, int);
static void map_kernel_region(uintptr_t, uintptr_t, size_t);

void paging_initialize(struct stivale2_struct_tag_memmap *mmap)
{
        /* disable kernel write access to read-only pages */
        /* asm volatile("movq %%cr0, %%rax;" */
        /*              "orq $(1 << 16), %%rax;" */
        /*              "movq %%rax, %%cr0;" */
        /*              : : : "rax"); */

        kernel_table = (uint64_t*)addr_ensure_higher(pmm_request_pages(1));
        memset(kernel_table, 0, sizeof(uint64_t) * 512);

        /* create kernel top-level entries so that all derived page tables
         * have access to the same structure and can share kernel mappings */
        for (size_t i = 256; i < 512; i++) {
                int64_t *child_entry = &kernel_table[i];

                /* allocate and zero out */
                uint64_t *child =
                        (uint64_t*)addr_ensure_higher(pmm_request_pages(1));
                memset(child, 0, 0x1000);

                *child_entry = PAGING_PRESENT | PAGING_WRITABLE | PAGING_USER |
                        addr_ensure_lower(child);
        }

        map_kernel_region(VADDR_HIGHER, 0x0, 0x100000000); /* 4G */
        map_kernel_region(VADDR_KERNEL, 0x0, 0x80000000); /* 2G */

        for (size_t i = 0; i < mmap->entries; i++) {
                /* map all memory map entries */
                const struct stivale2_mmap_entry *entry = &mmap->memmap[i];
                map_kernel_region(VADDR_HIGHER, entry->base, entry->length);
        }

        /* unmap NULL page */
        paging_unmap(kernel_table, NULL);

        paging_write_cr3(kernel_table);
}

void paging_map(uint64_t *table,
                void *vaddr,
                void *paddr,
                uint8_t flags)
{
        uint64_t *entry = paging_entry_get(table, vaddr, true);

        *entry = PAGING_PRESENT | flags |
                addr_page_align_down((uintptr_t)paddr);

        paging_flush_tlb(vaddr);
}

void paging_unmap(uint64_t *table, void *vaddr)
{
        uint64_t *entry = paging_entry_get(table, vaddr, false);
        if (entry) {
                *entry &= ~PAGING_PRESENT;
        }
}

uint64_t *paging_entry_get(uint64_t *table, void *vaddr, bool create)
{
        size_t lvl4_index, lvl3_index, lvl2_index, lvl1_index;
        map_index((uintptr_t)vaddr,
                  &lvl4_index,
                  &lvl3_index,
                  &lvl2_index,
                  &lvl1_index);

        uint64_t *lvl4 = get(table, lvl4_index, create);
        if (!lvl4)
                return NULL;
        uint64_t *lvl3 = get(lvl4, lvl3_index, create);
        if (!lvl3)
                return NULL;
        uint64_t *lvl2 = get(lvl3, lvl2_index, create);
        if (!lvl2)
                return NULL;

        return &lvl2[lvl1_index];
}

uint64_t *paging_create_empty(void)
{
        uint64_t *new_table = (uint64_t*)addr_ensure_higher(pmm_request_pages(1));
        memset(new_table, 0, sizeof(uint64_t) * 512);
        size_t half_off = sizeof(uint64_t) * 256;
        memcpy((void*)((uintptr_t)new_table + half_off),
               (void*)((uintptr_t)kernel_table + half_off), half_off);

        return new_table;
}

void paging_write_cr3(uint64_t *table)
{
        asm volatile("movq %0, %%cr3" : : "r" (addr_ensure_lower(table)));
}

void paging_flush_tlb(void *addr)
{
        asm volatile("invlpg (%0)" : : "r" (addr));
}

static void map_index(uintptr_t vaddr,
                      size_t *lvl4_index,
                      size_t *lvl3_index,
                      size_t *lvl2_index,
                      size_t *lvl1_index)
{
        /* 48 bits of the virtual address are translated,
           so we have access to 256TB
           the first 12 bits are used for indexing inside the page
           each consecutive 9-bit block is used to index into a page table
           */

        vaddr >>= 12;
        *lvl1_index = vaddr & 0x1ffUL; /* 0x1ff = 9-bit mask */
        vaddr >>= 9;
        *lvl2_index = vaddr & 0x1ffUL;
        vaddr >>= 9;
        *lvl3_index = vaddr & 0x1ffUL;
        vaddr >>= 9;
        *lvl4_index = vaddr & 0x1ffUL;
}

static uint64_t *get(uint64_t *parent,
                     size_t index,
                     bool create)
{
        if (parent == NULL)
                return NULL;

        uint64_t *child_entry = &parent[index];
        uint64_t *child;
        if (!(*child_entry & PAGING_PRESENT)) {
                if (!create)
                        return NULL;

                /* allocate and zero out */
                child = (uint64_t*)addr_ensure_higher(pmm_request_pages(1));
                memset(child, 0, 0x1000);

                *child_entry = PAGING_PRESENT | PAGING_USER |
                        PAGING_WRITABLE |
                        addr_ensure_lower(child);
        } else {
                /* child is present, all good (mask off first 12 bits) */
                child = (uint64_t*)addr_ensure_higher(*child_entry & ~0xfffUL);
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
                paging_map(kernel_table, (void*)(voffset + addr),
                           (void*)addr, 0);
        }
}
