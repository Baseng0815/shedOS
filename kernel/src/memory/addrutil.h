#ifndef _ADDR_UTIL
#define _ADDR_UTIL

#include <stdint.h>
#include <stddef.h>

extern const uintptr_t VADDR_HIGHER;
extern const uintptr_t VADDR_KERNEL;

inline uintptr_t vaddr_offset_higher(uintptr_t p);
inline uintptr_t vaddr_offset_lower(uintptr_t p);
inline uintptr_t vaddr_ensure_higher(uintptr_t p);
inline uintptr_t vaddr_ensure_lower(uintptr_t p);

inline uintptr_t addr_align_up(uintptr_t p, size_t a);
inline uintptr_t addr_align_down(uintptr_t p, size_t a);
inline uintptr_t addr_page_align_up(uintptr_t p);
inline uintptr_t addr_page_align_down(uintptr_t p);

#endif
