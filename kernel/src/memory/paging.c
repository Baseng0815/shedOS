#include "paging.h"

#include "pmm.h"
#include "../libk/memutil.h"
#include "../libk/printf.h"

#include "addrutil.h"

struct page_table *kernel_table;

/* get level indices */
static void map_index(uintptr_t, size_t*, size_t*, size_t*, size_t*);

static struct page_table *get(struct page_table*, size_t, bool);
static void _paging_copy_table(struct page_table*, struct page_table*, int);
static void map_kernel_region(uintptr_t, uintptr_t, size_t);

void paging_initialize(struct stivale2_struct_tag_memmap *mmap,
                       struct stivale2_struct_tag_framebuffer *fb)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target paging.\n");

        /* disable kernel write access to read-only pages */
        /* asm volatile("movq %%cr0, %%rax;" */
        /*              "orq $(1 << 16), %%rax;" */
        /*              "movq %%rax, %%cr0;" */
        /*              : : : "rax"); */

        kernel_table = (struct page_table*)
                vaddr_ensure_higher(pmm_request_pages(1));
        memset(kernel_table, 0, sizeof(struct page_table));

        printf(KMSG_LOGLEVEL_INFO,
               "Kernel table at %a\n", kernel_table);

        map_kernel_region(VADDR_HIGHER, 0x0, 0x100000000); /* 4G */
        map_kernel_region(VADDR_KERNEL, 0x0, 0x80000000); /* 2G */
        printf(KMSG_LOGLEVEL_INFO, "Mapped 4G/2G to higher/kernel\n");

        for (size_t i = 0; i < mmap->entries; i++) {
                /* map all memory map entries */
                const struct stivale2_mmap_entry *entry = &mmap->memmap[i];
                map_kernel_region(VADDR_HIGHER, entry->base, entry->length);
        }

        /* unmap NULL page */
        paging_unmap(kernel_table, NULL);

        printf(KMSG_LOGLEVEL_INFO, "Using new page table...\n");

        paging_write_cr3(kernel_table);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target paging.\n");
}

void paging_map(struct page_table *table,
                void *vaddr,
                void *paddr,
                uint8_t flags)
{
        uint64_t *entry = paging_entry_get(table, vaddr);

        *entry = PAGING_PRESENT | PAGING_USER | /* TODO remove user */
                flags |
                addr_page_align_down((uintptr_t)paddr);

        paging_flush_tlb(vaddr);
}

void paging_unmap(struct page_table *table, void *vaddr)
{
        uint64_t *entry = paging_entry_get(table, vaddr);
        if (entry) {
                *entry &= ~PAGING_PRESENT;
        }
}

uint64_t *paging_entry_get(struct page_table *table, void *vaddr)
{
        size_t lvl4_index, lvl3_index, lvl2_index, lvl1_index;
        map_index((uintptr_t)vaddr,
                  &lvl4_index,
                  &lvl3_index,
                  &lvl2_index,
                  &lvl1_index);

        struct page_table *lvl4 = get(table, lvl4_index, true);
        struct page_table *lvl3 = get(lvl4, lvl3_index, true);
        struct page_table *lvl2 = get(lvl3, lvl2_index, true);
        if (lvl2 == NULL)
                return NULL;

        return &lvl2->entries[lvl1_index];
}

void paging_copy_table(struct page_table *src, struct page_table **dst)
{
        *dst = (struct page_table*)vaddr_ensure_higher(pmm_request_pages(1));
        memset(*dst, 0, sizeof(struct page_table));
        _paging_copy_table(src, *dst, 2);
}

static void _paging_copy_table(struct page_table *src,
                        struct page_table *dst,
                        int level)
{
        if (level == 0) {
                memcpy(dst->entries, src->entries, sizeof(struct page_table));
        } else {
                for (size_t i = 0; i < 512; i++) {
                        struct page_table *src_new = get(src, i, false);
                        if (src_new == NULL)
                                continue;

                        struct page_table *dst_new = get(dst, i, true);
                        _paging_copy_table(src_new, dst_new, level - 1);
                }
        }
}

void paging_write_cr3(const struct page_table *table)
{
        asm volatile("movq %0, %%cr3" : : "r" (vaddr_ensure_lower(table)));
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

static struct page_table *get(struct page_table *parent,
                              size_t index,
                              bool create)
{
        if (parent == NULL)
                return NULL;

        uint64_t *child_entry = &parent->entries[index];
        struct page_table *child;
        if (!(*child_entry & PAGING_PRESENT)) {
                if (!create)
                        return NULL;

                /* allocate and zero out */
                child = (struct page_table*)
                        vaddr_ensure_higher(pmm_request_pages(1));
                memset(child, 0, 0x1000);

                *child_entry = PAGING_PRESENT | PAGING_USER |
                        PAGING_WRITABLE |
                        vaddr_ensure_lower(child);
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
