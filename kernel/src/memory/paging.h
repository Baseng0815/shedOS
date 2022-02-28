#ifndef _PAGING_H
#define _PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "../stivale2.h"

/* we use 4-level paging (top to bottom), each containing 512 children:
PML4
PML3
PML2
PML1 (its entries refer to the actual physical pages) */

/* schematic of how the address space looks
mappings:
0x0000000000000000-0x0000000100000000 -> 0xffff800000000000-0xffff800100000000
0x0000000000000000-0x0000000080000000 -> 0xfffffff800000000-0xffffffffffffffff
every additional mmap entry           -> 0xffff800000000000-max

regions:
0xffff800000000000-0xffff810000000000 (up to 1024G for mmap entries)
0xffff810000000000-0xffff810200000000 (4GB bump heap)
0xffff810200000000-0xffff810200001000 (page mapping for cow)
0xffffffff80000000-0xffffffffffffffff (kernel mapped to last 2G) */

/* a page table is exactly 4096 bytes large so it fits into a single frame */

/* page table flags */
#define PAGING_PRESENT  (1UL << 0) /* present */
#define PAGING_WRITABLE (1UL << 1) /* enable read/write */
#define PAGING_USER     (1UL << 2) /* enable user access */
#define PAGING_WTHROUGH (1UL << 3) /* enable write-through caching */
#define PAGING_CDISABLE (1UL << 4) /* disable cache */
#define PAGING_HUGE     (1UL << 7) /* huge pages */

extern uint64_t *kernel_table;

/* create a new page table for the kernel */
void paging_initialize(struct stivale2_struct_tag_memmap*);

void paging_map(uint64_t *page_table, void *vaddr, void *paddr, uint8_t flags);
void paging_unmap(uint64_t *page_table, void *vaddr);
uint64_t paging_get(uint64_t *page_table, void *vaddr);

/* create a new page table */
uint64_t *paging_create_empty(void);
/* create a shallow copy of table */
uint64_t *paging_copy(uint64_t *table);
/* make table read-only */
void paging_make_readonly(uint64_t *table);

void paging_write_cr3(uint64_t *page_table);
void paging_flush_tlb(void *addr);

#endif
