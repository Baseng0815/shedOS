#include "addrutil.h"

const uint64_t VADDR_HIGHER = 0xffff800000000000UL;
const uint64_t VADDR_KERNEL = 0xffffffff80000000UL;

uint64_t addr_offset_higher(uint64_t p)
{
        return p + VADDR_HIGHER;
}

uint64_t addr_offset_lower(uint64_t p)
{
        return p - VADDR_HIGHER;
}

uint64_t addr_ensure_higher(uint64_t p)
{
        return p | VADDR_HIGHER;
}

uint64_t addr_ensure_lower(uint64_t p)
{
        return p & ~VADDR_HIGHER;
}

uint64_t addr_align_up(uint64_t p, size_t a)
{
        return (p + a - 1) & ~(a - 1);
}

uint64_t addr_align_down(uint64_t p, size_t a)
{
        return p & ~(a - 1);
}

uint64_t addr_page_align_up(uint64_t p)
{
        return addr_align_up(p, 0x1000);
}

uint64_t addr_page_align_down(uint64_t p)
{
        return addr_align_down(p, 0x1000);
}

inline bool ptr_is_user(void *ptr)
{
        return (uint64_t)ptr <= 0x7ffffffffff0UL;
}
