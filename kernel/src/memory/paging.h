#ifndef _PAGING_H
#define _PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "../stivale2.h"

/* we use 4-level paging (top to bottom), each containing 512 children:
   lvl5 ("level 5" - top level)
   lvl4 (level 4)
   lvl3 (level 3)
   lvl2 (level 2)
   lvl1 (level 1) (this is the actual physical page)
   */

/* schematic of how the address space looks
mappings:
0x0000000000000000-0x0000000100000000 -> 0xffff800000000000-0xffff800100000000
0x0000000000000000-0x0000000080000000 -> 0xfffffff800000000-0xffffffffffffffff

regions:
0xffff800000000000-0xffff800100000000 (first 4G)
0xffff900000000000-0xffff900000000000 (4GB page heap)
0xffff900200000000-0xffff900300000000 (4GB norm heap)
0xfffffff800000000-0xffffffffffffffff (kernel mapped to last 2G)
*/

/* a page table is exactly 4096 bytes large so it fits into a single frame */
struct page_table {
        uint64_t entries[512];
} __attribute__((aligned(0x1000)));

enum {
        PAGING_PRESENT  = 1UL << 0, /* present */
        PAGING_WRITABLE = 1UL << 1, /* enable read/write */
        PAGING_USER     = 1UL << 2, /* enable user access */
        PAGING_WTHROUGH = 1UL << 3, /* enable write-through caching */
        PAGING_CDISABLE = 1UL << 4 /* disable cache */
};

extern struct page_table *kernel_table;

/* create a page table for the kernel */
void paging_initialize(struct stivale2_struct_tag_memmap*,
                       struct stivale2_struct_tag_framebuffer*);

bool paging_map(struct page_table*, void *vaddr, void *paddr, uint8_t flags);
void paging_unmap(struct page_table*, void *vaddr);
uint64_t *paging_entry_get(struct page_table*, void *vaddr);

void paging_write_cr3(const struct page_table*);
void paging_flush_tlb(void *addr);

#endif
