#include "pfa.h"

#include <printk.h> /* printk */
#include <string.h> /* memset */

#include "bitmap.h"

/* bytes */
static size_t           free_memory = 0;
static size_t           total_memory;
static struct bitmap    page_bitmap;
static uintptr_t        last_free = 0;

/* return the number of pages actually locked/unlocked */
static size_t lock_page(uintptr_t);
static size_t lock_pages(uintptr_t, size_t);
static size_t unlock_page(uintptr_t);
static size_t unlock_pages(uintptr_t, size_t);

extern uint64_t __KERNELSTART__;
extern uint64_t __KERNELEND__;

void pfa_initialize(struct efi_memory_map *mmap,
                    struct framebuffer *fb)
{
        printk(KMSG_LOGLEVEL_INFO,
               "Reached target pfa\n");

        /* find largest (conventional) descriptor to put the bitmap into.
           also get total amount of memory */
        size_t largest_size = 0;
        total_memory = 0;
        void *desc_paddr;

        /* modify memory map to include bss section */
        size_t num_desc = mmap->size / mmap->desc_size;

        /* TODO find block for bitmap that guarantees it doesn't contain kernel */
        for (size_t i = 0; i < num_desc; i++) {
                struct efi_memory_descriptor *desc =
                        (struct efi_memory_descriptor*)
                        ((uintptr_t)mmap->paddr + (i * mmap->desc_size));

                size_t block_size = desc->page_count * 0x1000;
                total_memory += block_size;

                if (desc->type != 7)
                        continue;

                if (block_size > largest_size) {
                        largest_size = block_size;
                        desc_paddr = desc->paddr;
                }
        }

        /* set up bitmap */
        size_t bitmap_len = total_memory / (0x1000 * 8) + 1;
        page_bitmap.buf = (uint8_t*)desc_paddr;
        page_bitmap.len = bitmap_len;
        printk(KMSG_LOGLEVEL_SUCC, "%x %d", page_bitmap.buf, page_bitmap.len / 4096);

        /* lock all pages */
        memset(page_bitmap.buf, 0xff, page_bitmap.len);

        /* unlock all pages that are of type EfiConventionalMemory */
        for (size_t i = 0; i < num_desc; i++) {
                struct efi_memory_descriptor *desc =
                        (struct efi_memory_descriptor*)
                        (mmap->paddr + (i * mmap->desc_size));

                if (desc->type == 7) {
                        unlock_pages((uintptr_t)desc->paddr, desc->page_count);
                }
        }

        /* lock bitmap pages */
        lock_pages((uintptr_t)page_bitmap.buf, page_bitmap.len / 0x1000);

        /* lock kernel pages */
        uintptr_t   kstart = &__KERNELSTART__;
        size_t      klen = (uintptr_t)&__KERNELEND__ - kstart;
        lock_pages(kstart, klen);

        printk(KMSG_LOGLEVEL_INFO,
               "Total memory: %dKiB (%d pages).\n"
               "Total usable amount of memory: %dKiB (%d pages).\n",
               total_memory, total_memory / 0x1000,
               free_memory, free_memory / 0x1000);

        printk(KMSG_LOGLEVEL_SUCC,
               "Finished target pfa\n");
}

void *pfa_request_page()
{
        for (size_t pi = last_free; pi < total_memory / 0x1000; pi++) {
                if (!bitmap_isset(&page_bitmap, pi)) {
                        last_free = pi + 1;
                        uintptr_t addr = pi * 0x1000;
                        lock_page(addr);
                        return (void*)(addr);
                }
        }

        return NULL;
}

void pfa_release_page(void *page)
{
        uintptr_t p = (uintptr_t)page;
        if (p < last_free) {
                last_free = p;
        }
        unlock_page(p);
}

size_t lock_page(uintptr_t paddr)
{
        size_t pi = paddr / 0x1000;
        if (!bitmap_isset(&page_bitmap, pi)) {
                bitmap_set(&page_bitmap, pi);
                free_memory -= 0x1000;
                return 1;
        }

        return 0;
}

size_t lock_pages(uintptr_t paddr, size_t n)
{
        size_t locked = 0;
        for (size_t i = 0; i < n; i++) {
                locked += lock_page(paddr + 0x1000 * i);
        }

        return locked;
}

size_t unlock_page(uintptr_t paddr)
{
        size_t pi = paddr / 0x1000;
        if (bitmap_isset(&page_bitmap, pi)) {
                bitmap_unset(&page_bitmap, pi);
                free_memory += 0x1000;
                return 1;
        }

        return 0;
}

size_t unlock_pages(uintptr_t paddr, size_t n)
{
        size_t unlocked = 0;
        for (size_t i = 0; i < n; i++) {
                unlocked += unlock_page(paddr + 0x1000 * i);
        }
        return unlocked;
}

size_t pfa_get_mem_total()
{
        return total_memory;
}

size_t pfa_get_mem_free()
{
        return free_memory;
}
