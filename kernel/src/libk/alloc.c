#include "alloc.h"

#include "../memory/addrutil.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"

static const uintptr_t BUMP_LOWER_BOUND = 0xffff810000000000UL;
static const uintptr_t BUMP_UPPER_BOUND = 0xffff810200000000UL;

/* a bump allocator works like a stack with element alignment, meaning
 * we can only free in LIFO order or free the whole pool at once */
struct bump_alloc {
        uintptr_t start;
        uintptr_t end;
        uintptr_t ptr;
} bump = {
        .start  = BUMP_UPPER_BOUND,
        .end    = BUMP_UPPER_BOUND,
        .ptr    = BUMP_UPPER_BOUND
};

void *balloc(size_t n, size_t alignment)
{
        uintptr_t new_ptr = bump.ptr - n;

        if (alignment > 0) {
                new_ptr = addr_align_down(new_ptr, alignment);
        }

        if (new_ptr < bump.start) {
                /* grow bump */
                size_t diff = addr_page_align_up(bump.start - new_ptr);
                uintptr_t new_start = bump.start - diff;
                if (new_start < BUMP_LOWER_BOUND) {
                        return NULL;
                }

                vmm_request_at(kernel_table,
                               (void*)(bump.start - diff),
                               diff / 0x1000,
                               PAGING_WRITABLE);
                bump.start -= diff;
        }

        bump.ptr = new_ptr;
        return (void*)new_ptr;
}

void bfree(void)
{
        uintptr_t begin = addr_page_align_down(bump.end);
        uintptr_t end   = addr_page_align_up(bump.start);
        vmm_release_at(kernel_table, (void*)begin,
                       (end - begin) / 0x1000);
}

void *palloc(size_t n)
{
        return (void*)addr_offset_higher((uint64_t)pmm_request_pages(n));
}

void pfree(void *ptr, size_t n)
{
        vmm_release_at(kernel_table, ptr, n);
}
