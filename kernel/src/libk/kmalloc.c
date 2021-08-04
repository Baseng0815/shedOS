#include "kmalloc.h"

#include "memutil.h"
#include "../memory/vmm.h"
#include "../memory/addrutil.h"

#include "printf.h"

struct mheap_hdr {
        bool    is_free;
        size_t  size;
} __attribute__((aligned(16)));

/* page heap starting 16T above the higher half */
static const uintptr_t pheap_start  = 0xffff900000000000;
static uintptr_t pnext_free         = pheap_start;

/* kernel heap starting 16T+4G above the higher half */
static const uintptr_t mheap_start  = 0xffff900100000000;
static struct mheap_hdr *mheap_head = (struct mheap_hdr*)mheap_start;

static void *kmalloc_split_hdr(struct mheap_hdr*, size_t alloc_size);

void kmalloc_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target kmalloc.\n");
        printf(KMSG_LOGLEVEL_INFO, "Kernel page heap at %a, mem heap at %a\n",
               pheap_start, mheap_start);

        const size_t initial_heap_size = 0x1000;
        vmm_request_at(kernel_table, mheap_head, 1, false, true);
        mheap_head->is_free = true;
        mheap_head->size    = initial_heap_size - 2 * sizeof(struct mheap_hdr);

        struct mheap_hdr *tail = (struct mheap_hdr*)
                ((uintptr_t)mheap_head +
                 mheap_head->size +
                 sizeof(struct mheap_hdr));

        tail->is_free   = false;
        tail->size      = 0;

        printf(KMSG_LOGLEVEL_OKAY, "Finished target kmalloc.\n");
}

void *kpmalloc()
{
        void *memory = pnext_free;

        /* find next free virtual address */
        struct pt_entry *entry;
        do {
                pnext_free++;
                entry = paging_entry_get(kernel_table, pnext_free);
        } while (entry->present);

        return memory;
}

void *kpzmalloc()
{
        void *memory = kpmalloc();
        memset(memory, 0, 0x1000);
        return memory;
}

void kpfree(void *memory)
{
        vmm_release_at(kernel_table, memory, 1);
        if (memory < pnext_free)
                pnext_free = memory;
}

/* 2 cases
 * - large enough header exists (split it)
 * - large enough header does not exist (append pages)
 */

void *kmalloc(size_t size)
{
        /* this is shit cause runtime is O(n). Too bad!
           will probably do buddy allocation later */
        size_t alloc_size = addr_align_up(size, 16) + sizeof(struct mheap_hdr);

        struct mheap_hdr *hdr = mheap_head;
        while (hdr->size != 0) {
                if (hdr->size >= alloc_size) {
                        /* large enough section found */
                        return kmalloc_split_hdr(hdr, alloc_size);
                }

                hdr = (struct mheap_hdr*)
                        ((uintptr_t)hdr + hdr->size + sizeof(struct mheap_hdr));
        }

        /* no section is large enough; append necessary pages */
        size_t additional_pages = addr_page_align_up(alloc_size) / 0x1000;
        vmm_request_at(kernel_table,
                       (uintptr_t)hdr + sizeof(struct mheap_hdr),
                       additional_pages, false, true);
        hdr->is_free    = true;
        hdr->size       = additional_pages * 0x1000 - sizeof(struct mheap_hdr);

        struct mheap_hdr *new_null_hdr = (struct mheap_hdr*)
                ((uintptr_t)hdr +
                 hdr->size +
                 sizeof(struct mheap_hdr));

        new_null_hdr->is_free   = false;
        new_null_hdr->size      = 0;

        return kmalloc_split_hdr(hdr, alloc_size);
}

static void *kmalloc_split_hdr(struct mheap_hdr *hdr, size_t alloc_size)
{
        struct mheap_hdr *new_hdr = (struct mheap_hdr*)
                ((uintptr_t)hdr + alloc_size);
        new_hdr->is_free = true;
        new_hdr->size = hdr->size - alloc_size;

        hdr->is_free = false;
        hdr->size = alloc_size - sizeof(struct mheap_hdr);

        return (void*)((uintptr_t)hdr + sizeof(struct mheap_hdr));
}
