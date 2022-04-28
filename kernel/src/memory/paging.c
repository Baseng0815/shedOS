#include "paging.h"

#include "../libk/memutil.h"

#include "addrutil.h"

#ifdef DEBUG
#include "../libk/printf.h"
#endif

#include "../libk/alloc.h"

uint64_t *kernel_table;

static void paging_map_recursive(uint8_t, uint64_t*, uint64_t, uint64_t, uint8_t);
static void paging_unmap_recursive(uint8_t, uint64_t*, uint64_t);
static uint64_t paging_get_recursive(uint8_t, uint64_t*, uint64_t);
static void paging_copy_recursive(uint64_t*, uint64_t*, uint8_t);
static void paging_make_readonly_recursive(uint64_t*, uint8_t, uint64_t);

void paging_initialize(struct stivale2_struct_tag_memmap *mmap)
{
        kernel_table = palloc(1);
        memset(kernel_table, 0, 0x1000);

        /* create kernel top-level entries so that all derived page tables
         * have access to the same structure and can share kernel mappings */
        for (size_t i = 256; i < 512; i++) {
                /* allocate and zero out */
                uint64_t child = (uint64_t)palloc(1);
                memset((void*)child, 0, 0x1000);

                kernel_table[i] = PAGING_PRESENT | addr_offset_lower(child);
        }

        /* map 4G to VADDR_HIGHER (1G pages) */
        uint64_t *parent = (uint64_t*)
                addr_ensure_higher((kernel_table[256] & ~0xfffUL));
        for (size_t i = 0; i < 4; i++) {
                parent[i] = (uint64_t)(i * 0x40000000UL)
                        | PAGING_PRESENT
                        | PAGING_WRITABLE
                        | PAGING_HUGE;
        }

        /* map 2G to VADDR_KERNEL (1G pages) */
        parent = (uint64_t*)addr_ensure_higher((kernel_table[511] & ~0xfffUL));
        for (size_t i = 0; i < 2; i++) {
                parent[510 + i] = (uint64_t)(i * 0x40000000UL)
                        | PAGING_PRESENT
                        | PAGING_WRITABLE
                        | PAGING_HUGE;
        }

        for (size_t i = 0; i < mmap->entries; i++) {
                /* map all memory map entries to VADDR_HIGHER (4K pages) */
                const struct stivale2_mmap_entry *entry = &mmap->memmap[i];
                for (uint64_t paddr = entry->base;
                     paddr < entry->base + entry->length;
                     paddr += 0x1000) {
                        /* everything < 4G is already mapped as 1G page */
                        if (paddr < 0x100000000)
                                continue;

                        paging_map(kernel_table,
                                   (void*)(VADDR_HIGHER + paddr),
                                   (void*)paddr,
                                   PAGING_WRITABLE);
                }
        }

        /* unmap NULL page */
        paging_unmap(kernel_table, NULL);

        paging_write_cr3(kernel_table);
}

void paging_map(uint64_t *table, void *vaddr, void *paddr, uint8_t flags)
{
        paging_map_recursive(4, table,
                             (uint64_t)vaddr >> 12,
                             (uint64_t)paddr & ~0xfff, flags);

        paging_flush_tlb(vaddr);

#ifdef DEBUG
        printf(KMSG_LOGLEVEL_INFO, "paging: mapped %x to %x\n", vaddr, paddr);
#endif
}

void paging_map_recursive(uint8_t level, uint64_t *table, uint64_t vaddr,
                          uint64_t paddr, uint8_t flags)
{
        size_t index = (vaddr >> 9 * (level - 1)) & 0x1ff;
        if (level == 1) {
                table[index] = paddr | flags | PAGING_PRESENT;
                return;
        }

        uint64_t child = table[index];
        if (!(child & PAGING_PRESENT)) {
                /* create if not existing */
                child = (uint64_t)palloc(1);
                memset((void*)child, 0, 0x1000);
                table[index] = addr_offset_lower(child) | PAGING_PRESENT;

                /* we give write and user access for every
                 * non-final level in userspace */
                if (vaddr < VADDR_HIGHER)
                        table[index] |= PAGING_WRITABLE | PAGING_USER;
        }

        table = (uint64_t*)addr_ensure_higher(child & ~0xfffUL);
        return paging_map_recursive(level - 1, table, vaddr, paddr, flags);
}

void paging_unmap(uint64_t *table, void *vaddr)
{
        paging_unmap_recursive(4, table, (uint64_t)vaddr >> 12);

#ifdef DEBUG
        printf(KMSG_LOGLEVEL_INFO, "paging: unmapped %x\n", vaddr);
#endif
}

void paging_unmap_recursive(uint8_t level, uint64_t *table, uint64_t vaddr)
{
        size_t index = (vaddr >> 9 * (level - 1)) & 0x1ff;
        if (level == 1) {
                table[index] = 0;
                return;
        }

        uint64_t child = table[index];
        if (!(child & PAGING_PRESENT))
                return;

        table = (uint64_t*)addr_ensure_higher(child & ~0xfffUL);
        paging_unmap_recursive(level - 1, table, vaddr);
}

uint64_t paging_get(uint64_t *table, void *vaddr)
{
        return paging_get_recursive(4, table,
                                    (uint64_t)vaddr >> 12);
}

uint64_t paging_get_recursive(uint8_t level,
                              uint64_t *table,
                              uint64_t vaddr)
{
        size_t index = (vaddr >> 9 * (level - 1)) & 0x1ff;
        uint64_t child = table[index];
        if (!(child & PAGING_PRESENT))
                return 0;
        if (level == 1 || child & PAGING_HUGE)
                return child;

        table = (uint64_t*)addr_ensure_higher(child & ~0xfffUL);
        return paging_get_recursive(level - 1, table, vaddr);
}

uint64_t *paging_create_empty(void)
{
        uint64_t *new_table = palloc(1);
        memset(new_table, 0, 0x1000);
        memcpy((void*)((uint64_t)new_table + 0x800),
               (void*)((uint64_t)kernel_table + 0x800), 0x800);

#ifdef DEBUG
        printf(KMSG_LOGLEVEL_INFO, "paging: new empty table at %x\n",
               new_table);
#endif

        return new_table;
}

uint64_t *paging_copy(uint64_t *parent)
{
        uint64_t *new_table = paging_create_empty();
        paging_copy_recursive(new_table, parent, 4);

#ifdef DEBUG
        printf(KMSG_LOGLEVEL_INFO, "paging: copied %x to %x\n",
               parent, new_table);
#endif

        return new_table;
}

void paging_copy_recursive(uint64_t *dst,
                           uint64_t *src,
                           uint8_t level)
{
        if (level == 1) {
                memcpy(dst, src, 0x1000);
                return;
        }

        for (size_t i = 0; i < (level == 4 ? 256 : 512); i++) {
                if (!(src[i] & PAGING_PRESENT))
                        continue;

                uint64_t *new_src = (uint64_t*)
                        addr_ensure_higher(src[i] & ~0xfffUL);
                uint64_t *new_dst = palloc(1);
                paging_copy_recursive((void*)new_dst,
                                      (void*)new_src,
                                      level - 1);
                dst[i] = addr_offset_lower((uint64_t)new_dst)
                        | (src[i] & 0xfff);
        }
}

void paging_make_readonly(uint64_t *table)
{
        paging_make_readonly_recursive(table, 4, 0);

#ifdef DEBUG
        printf(KMSG_LOGLEVEL_INFO, "paging: made %x readonly\n",
               table);
#endif
}

void paging_make_readonly_recursive(uint64_t *table,
                                    uint8_t level,
                                    uint64_t vaddr)
{
        if (level == 1) {
                for (size_t i = 0; i < 512; i++) {
                        uint64_t final_vaddr = ((vaddr << 9) | i) << 12;
                        paging_flush_tlb((void*)final_vaddr);
                        table[i] &= ~PAGING_WRITABLE;
                }
                return;
        }

        for (size_t i = 0; i < (level == 4 ? 256 : 512); i++) {
                if (!(table[i] & PAGING_PRESENT))
                        continue;

                uint64_t *child = (uint64_t*)
                        addr_ensure_higher((table[i] & ~0xfffUL));
                paging_make_readonly_recursive(child, level - 1,
                                               (vaddr << 9) | i);
        }
}

void paging_write_cr3(uint64_t *table)
{
        asm volatile("movq %0, %%cr3"
                     :
                     : "r" (addr_ensure_lower((uint64_t)table)));
}

void paging_flush_tlb(void *addr)
{
        asm volatile("invlpg (%0)"
                     :
                     : "b" (addr) : "memory");
}
