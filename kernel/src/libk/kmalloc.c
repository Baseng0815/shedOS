#include "kmalloc.h"

#include "memutil.h"
#include "../memory/vmm.h"
#include "../memory/bitmap.h"

#include "printf.h"

/* we use a bitmap allocator with a fixed-size of 8 pages to keep track
   of open slots (allows 64G of allocations) */

static const size_t pheap_pcount = 8;

/* page heap starting 4G + 32K above the higher half */
static const uintptr_t pheap_bot = 0xffff800100000000 + 0x1000 * pheap_pcount;
static const struct bitmap pbitmap = {
        .buf = (uint8_t*)pheap_bot - 0x1000 * pheap_pcount,
        .len = 0x1000 * pheap_pcount,
};

static size_t plast_free = 0;

void malloc_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target malloc.\n");

        printf(KMSG_LOGLEVEL_INFO, "Kernel page heap at %a\n", pheap_bot);

        vmm_request_at(kernel_table, pbitmap.buf, pheap_pcount);
        printf(KMSG_LOGLEVEL_OKAY, "XDDDDDDD\n");

        /* zero out page bitmap */
        memset(pbitmap.buf, 0, pbitmap.len);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target malloc.\n");
}

void *kpmalloc(size_t count)
{
        for (size_t pi = plast_free; pi < pheap_pcount * 0x1000; pi++) {
                bool is_free = true;
                for (size_t c = 0; c < count; c++) {
                        if (bitmap_isset(&pbitmap, pi)) {
                                is_free = false;
                                break;
                        }
                }

                if (is_free) {
                        plast_free = pi + count;
                        uintptr_t addr = pi * 0x1000;
                        for (size_t i = pi; i < pi + count; i++) {
                                bitmap_set(&pbitmap, i);
                        }

                        return (void*)addr;
                }
        }

        return NULL;
}

void *kpzmalloc(size_t count)
{
        void *memory = kpmalloc(count);
        memset(memory, 0, count * 0x1000);
        return memory;
}
