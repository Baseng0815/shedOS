#include "alloc.h"

#include "../memory/addrutil.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "src/libk/util.h"
#include "src/memory/paging.h"
#include <string.h>

#define CHUNK_HDR(addr) \
        (struct chunk_hdr*)((uintptr_t)addr - sizeof(struct chunk_hdr))
#define NEXT_HDR(hdr) \
        (struct chunk_hdr*)((uintptr_t)hdr + hdr->size + sizeof(struct chunk_hdr));
#define PREV_HDR(hdr) \
        (struct chunk_hdr*)((uintptr_t)hdr - hdr->size - sizeof(struct chunk_hdr));

/* 4 GiB virtual memory */
static const uintptr_t KERNEL_HEAP_VIRTUAL_START    = 0xffff810000000000UL;
static const uintptr_t KERNEL_HEAP_VIRTUAL_END      = 0xffff810200000000UL;

/* a simple allocator managing a dynamically growing chunk of memory
 * using a linked list approach with block splitting/coalescing */

/* alloc.start
 * HEAD(size_prev=0) | H1 | C1 | H2 | C2 | ... | Hn | Cn | TAIL(size=0, in_use=true)
 *
 * allocating:
 * HEAD | H1(size=100) | C1 | TAIL -> HEAD | H1'(size=30) | C1' | H2(size=56) | C2 | TAIL
 */

/* the chunk header sits before every allocated block and contains metadata */
struct chunk_hdr {
        bool in_use;
        size_t size;
        size_t size_prev;
};

struct alloc {
        uintptr_t start;
        uintptr_t size;
};

static struct alloc alloc = {
        .start  = KERNEL_HEAP_VIRTUAL_START,
        .size   = 0x1000 // one page to begin with
};

void _malloc_init(void)
{
        vmm_request_at(kernel_table,
                       (void*)alloc.start,
                       alloc.size / 0x1000,
                       PAGING_WRITABLE);

        struct chunk_hdr *head = (struct chunk_hdr*)alloc.start;
        head->size      = alloc.size - 2 * sizeof(struct chunk_hdr);
        head->in_use    = false;
        head->size_prev = 0;

        struct chunk_hdr *tail = NEXT_HDR(head);
        tail->size      = 0;
        tail->in_use    = true;
        tail->size_prev = head->size;
}

void *malloc(size_t n, size_t alignment)
{
        if (n == 0) {
                // we don't want empty allocations
                return NULL;
        }

        /* make size contain some extra memory for the new header */
        size_t size = n + sizeof(struct chunk_hdr);
        struct chunk_hdr *hdr = (struct chunk_hdr*)alloc.start;
        while (hdr->size != 0 && (hdr->in_use || hdr->size < size)) {
                hdr = NEXT_HDR(hdr);
        }

        if (hdr->size == 0) {
                /* end reached => no free chunk found => append new memory */
                size_t new_page_count = addr_page_align_up(size) / 0x1000;
                uintptr_t region_end = ((uintptr_t)hdr + sizeof(struct chunk_hdr));
                if (region_end > KERNEL_HEAP_VIRTUAL_END) {
                        /* no memory left :( */
                        return NULL;
                }

                vmm_request_at(kernel_table,
                               (void*)region_end,
                               new_page_count,
                               PAGING_WRITABLE);

                hdr->size = new_page_count * 0x1000 - sizeof(struct chunk_hdr);
                struct chunk_hdr *new_tail = NEXT_HDR(hdr);
                new_tail->size      = 0;
                new_tail->in_use    = true;
                new_tail->size_prev = hdr->size;
        }

        /* hdr now points to a large enough chunk */

        size_t size_old = hdr->size;
        hdr->size = size - sizeof(struct chunk_hdr);
        hdr->in_use = true;

        struct chunk_hdr *hdr_new = NEXT_HDR(hdr);
        hdr_new->size       = size_old - size;
        hdr_new->in_use     = false;
        hdr_new->size_prev  = hdr->size;

        return (void*)((uintptr_t)hdr + sizeof(struct chunk_hdr));
}

void *zmalloc(size_t n)
{
        void *mem = malloc(n, 0);
        memset(mem, 0, n);
        return mem;
}

void mfree(void *addr)
{
        struct chunk_hdr *hdr = CHUNK_HDR(addr);
        hdr->in_use = false;

        if (hdr->size_prev != 0) {
                /* merge with previous chunk if hdr is not the head */
                struct chunk_hdr *hdr_prev = PREV_HDR(hdr);
                if (!hdr_prev->in_use) {
                        /* merge with previous chunk */
                        hdr_prev->size += hdr->size + sizeof(struct chunk_hdr);
                        hdr = hdr_prev;
                }
        }

        struct chunk_hdr *hdr_next = NEXT_HDR(hdr);
        if (!hdr_next->in_use && hdr_next->size != 0) {
                /* merge with next chunk if next chunk is not last */
                hdr->size += hdr_next->size + sizeof(struct chunk_hdr);
                struct chunk_hdr *nextnext = NEXT_HDR(hdr_next);
                nextnext->size_prev = hdr_next->size;
        }
}

void *palloc(size_t n)
{
        return (void*)addr_offset_higher((uint64_t)pmm_request_pages(n));
}

void *zpalloc(size_t n)
{
        void *mem = palloc(n);
        memset(mem, 0, n);
        return mem;
}

void pfree(void *ptr, size_t n)
{
        pmm_release_pages((void*)addr_ensure_lower((uint64_t)ptr), n);
}
