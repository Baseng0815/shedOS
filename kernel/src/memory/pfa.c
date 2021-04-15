#include "pfa.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"

#include "bitmap.h"

/* bytes */
static struct bitmap    page_bitmap;
static uintptr_t        last_free = 0x0;

size_t free_memory;
size_t total_memory;

/* return the number of pages actually locked/unlocked */
static size_t lock_page(uintptr_t);
static size_t lock_pages(uintptr_t, size_t);
static size_t unlock_page(uintptr_t);
static size_t unlock_pages(uintptr_t, size_t);

void pfa_initialize(struct stivale2_struct_tag_memmap *mmap)
{
        printf(KMSG_LOGLEVEL_INFO,
               "Reached target pfa.\n");

        /* find largest (conventional) memory map entry to put the bitmap into.
           also get total amount of memory */
        total_memory = free_memory = 0;
        struct stivale2_mmap_entry *largest_entry = NULL;

        for (size_t i = 0; i < mmap->entries; i++) {
                struct stivale2_mmap_entry *entry = &mmap->memmap[i];

                total_memory += entry->length;

                /* we only care about memory of type USABLE for now */
                if (entry->type != STIVALE2_MMAP_USABLE)
                        continue;

                if (!largest_entry || entry->length > largest_entry->length) {
                        largest_entry = entry;
                }
        }


        /* set up bitmap */
        size_t bitmap_len = total_memory / (0x1000 * 8) + 1;
        page_bitmap.buf = (uint8_t*)largest_entry->base;
        page_bitmap.len = bitmap_len;
        printf(KMSG_LOGLEVEL_INFO,
               "Bitmap starting at %x with length of %d bytes\n",
               page_bitmap.buf, page_bitmap.len);

        /* lock all pages */
        memset(page_bitmap.buf, 0xff, page_bitmap.len);

        /* unlock all pages that are USABLE */
        for (size_t i = 0; i < mmap->entries; i++) {
                struct stivale2_mmap_entry *entry = &mmap->memmap[i];

                if (entry->type == STIVALE2_MMAP_USABLE) {
                        unlock_pages((uintptr_t)entry->base,
                                     entry->length / 0x1000);
                }
        }

        /* lock bitmap pages */
        lock_pages((uintptr_t)page_bitmap.buf, page_bitmap.len / 0x1000);

        printf(KMSG_LOGLEVEL_INFO,
               "Total memory: %dKiB (%d pages).\n"
               "Total usable amount of memory: %dKiB (%d pages).\n",
               total_memory / 0x400, total_memory / 0x1000,
               free_memory / 0x400, free_memory / 0x1000);

        printf(KMSG_LOGLEVEL_OKAY,
               "Finished target pfa.\n");
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
