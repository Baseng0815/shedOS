#ifndef _PAGING_H
#define _PAGING_H

#include <stdbool.h>
#include <stdint.h>

#include "../stivale2.h"

#define VADDR_HIGHER 0xffff800000000000
#define VADDR_KERNEL 0xffffffff80000000
#define VADDR_OFFSET(p) ((uintptr_t)(p + VADDR_HIGHER))
#define VADDR_ENSURE_HIGHER(p) (p < VADDR_HIGHER ? p + VADDR_HIGHER : p)

/* we use 4-level paging (top to bottom), each containing 512 children:
   pml4 (page map level 4, top structure)
   pdp  (page directory pointer)
   pd   (page directory)
   pt   (page table)
   p    (page) (this is the physical frame address)
   */

struct pt_entry {
        uint64_t present        : 1; /* 1 => page is in physical memory */
        uint64_t writable       : 1; /* 1 => read/write, 0 => read only */
        uint64_t user_access    : 1; /* 1 => page can be accessed by all */
        uint64_t write_through  : 1; /* 1 => use write-through cache strategy */
        uint64_t cache_disabled : 1; /* 1 => page will not be cached */
        uint64_t accessed       : 1; /* set to 1 when page is accessed */
        uint64_t ignore0        : 1; /* can't use, is reserved */
        uint64_t large_pages    : 1; /* 0 => 4KiB page size */
        uint64_t ignore1        : 1; /* can't use, is reserved */
        uint64_t available      : 3; /* here we can store anything we want */
        uint64_t addr           : 52; /* either physical memory or other
                                         pd_entry */
};

/* a page table is exactly 4096 bytes large so it fits into a single frame */
struct page_table {
        /* we make no difference between pd_entries and actual pages */
        struct pt_entry entries[512];
} __attribute__((aligned(0x1000)));

extern struct page_table *kernel_table;

/* create a page table for the kernel */
void paging_initialize(struct stivale2_struct_tag_memmap*,
                       struct stivale2_struct_tag_framebuffer*);

void paging_map(struct page_table*, void*, void*);

/* we don't care about performance now and just reload CR3 every time */
void paging_update(const struct page_table*);

#endif
