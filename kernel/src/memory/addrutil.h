#ifndef _ADDR_UTIL
#define _ADDR_UTIL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern const uintptr_t VADDR_HIGHER;
extern const uintptr_t VADDR_KERNEL;

inline uintptr_t addr_offset_higher(uintptr_t p);
inline uintptr_t addr_offset_lower(uintptr_t p);
inline uintptr_t addr_ensure_higher(uintptr_t p);
inline uintptr_t addr_ensure_lower(uintptr_t p);

/* a needs to be a multiple of 2 */
inline uintptr_t addr_align_up(uintptr_t p, size_t a);
inline uintptr_t addr_align_down(uintptr_t p, size_t a);
inline uintptr_t addr_page_align_up(uintptr_t p);
inline uintptr_t addr_page_align_down(uintptr_t p);

inline bool ptr_is_user(void *ptr);

#endif
