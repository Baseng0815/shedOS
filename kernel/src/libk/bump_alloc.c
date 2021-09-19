#include "bump_alloc.h"

#include "../memory/addrutil.h"
#include "../memory/vmm.h"

static const uintptr_t LOWER_BOUND = 0xffff800100000000;
static const uintptr_t UPPER_BOUND = 0xffff8ffffffff000;

/* initial size of 1 page (4K) */
static struct bump_alloc bump = {
        .start      = UPPER_BOUND,
        .end        = UPPER_BOUND,
        .bump_ptr   = UPPER_BOUND
};

void *bump_alloc(size_t n, size_t alignment)
{
        uintptr_t new_ptr = bump.bump_ptr - n;
        new_ptr = addr_align_down(new_ptr, alignment);
        if (new_ptr < bump.start) {
                /* grow bump */
                size_t pc = addr_page_align_up(bump.start - new_ptr) / 0x1000;
                uintptr_t new_start = bump.start - pc * 0x1000;
                if (new_start < LOWER_BOUND) {
                        return NULL;
                }

                vmm_request_at(kernel_table,
                               bump.start - pc * 0x1000,
                               pc,
                               0);
                bump.start -= pc * 0x1000;
        }

        bump.bump_ptr = new_ptr;
        return (void*)new_ptr;
}

void bump_free_all()
{
        bump.start = UPPER_BOUND;
        bump.bump_ptr = bump.end;
}
