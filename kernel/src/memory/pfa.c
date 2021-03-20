#include "pfa.h"

#include <printk.h> /* printk */
#include <string.h> /* memset */

#include "bitmap.h"

/* bytes */
static size_t free_memory = 0;
static size_t total_memory = 0;
static struct bitmap page_bitmap;

/* return the number of pages actually locked/unlocked */
static size_t lock_page(uintptr_t);
static size_t lock_pages(uintptr_t, size_t);
static size_t unlock_page(uintptr_t);
static size_t unlock_pages(uintptr_t, size_t);

extern uint64_t __KERNELSTART__;
extern uint64_t __KERNELEND__;

void pfa_initialize(struct efi_memory_map *mmap)
{
        printk(KMSG_LOGLEVEL_INFO,
               "Reached target pfa\n");

        /* find largest (conventional) descriptor to put the bitmap into.
           also get total amount of memory */
        size_t largest_size = 0;
        void *desc_paddr;

        /* modify memory map to include bss section */
        size_t num_desc = mmap->size / mmap->desc_size;

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

        /* unlock all pages and set free mem*/
        free_memory = total_memory;
        for (size_t pi = 0; pi < page_bitmap.len; pi++) {
                page_bitmap.buf[pi] = 0;
        }

        /* lock bitmap pages */
        lock_pages((uintptr_t)page_bitmap.buf, page_bitmap.len / 0x1000);

        /* lock pages used by kernel */
        uintptr_t kernel_start  = &__KERNELSTART__;
        uintptr_t kernel_end    = &__KERNELEND__;
        size_t kernel_pagecount = (kernel_end - kernel_start) / 0x1000;
        printk(KMSG_LOGLEVEL_INFO,
               "KERNELSTART=%x, KERNELEND=%x, size=%d pages\n",
               &__KERNELSTART__, &__KERNELEND__, kernel_pagecount);
        lock_pages(kernel_start, kernel_pagecount);

        /* lock all pages that are not of type EfiConventionalMemory */
        for (size_t i = 0; i < num_desc; i++) {
                struct efi_memory_descriptor *desc =
                        (struct efi_memory_descriptor*)
                        (mmap->paddr + (i * mmap->desc_size));

                if (desc->page_count == 0)
                        continue;

                if (desc->type != 7) {
                        lock_pages((uintptr_t)desc->paddr, desc->page_count);
                }
        }

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
        for (size_t pi = 0; pi < total_memory / 0x1000; pi++) {
                if (!bitmap_isset(&page_bitmap, pi)) {
                        lock_page(pi * 0x1000);
                        return (void*)(pi * 0x1000);
                }
        }

        return NULL;
}

void pfa_release_page(void *page)
{
        unlock_page((uintptr_t)page);
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
