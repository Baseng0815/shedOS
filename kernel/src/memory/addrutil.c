#include "addrutil.h"

const uintptr_t VADDR_HIGHER = 0xffff800000000000UL;
const uintptr_t VADDR_KERNEL = 0xffffffff80000000UL;

uintptr_t vaddr_offset_higher(uintptr_t p)
{
        return p + VADDR_HIGHER;
}

uintptr_t vaddr_offset_lower(uintptr_t p)
{
        return p - VADDR_HIGHER;
}

uintptr_t vaddr_ensure_higher(uintptr_t p)
{
        return p | VADDR_HIGHER;
}

uintptr_t vaddr_ensure_lower(uintptr_t p)
{
        return p & ~VADDR_HIGHER;
}

uintptr_t addr_align_up(uintptr_t p, size_t a)
{
        if (p % a == 0) return p;
        else            return (p / a + 1) * a;
}

uintptr_t addr_align_down(uintptr_t p, size_t a)
{
        return (p / a) * a;
}

uintptr_t addr_page_align_up(uintptr_t p)
{
        return addr_align_up(p, 0x1000);
}

uintptr_t addr_page_align_down(uintptr_t p)
{
        return addr_align_down(p, 0x1000);
}
