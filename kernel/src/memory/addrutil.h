#ifndef _ADDR_UTIL
#define _ADDR_UTIL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern const uint64_t VADDR_HIGHER;
extern const uint64_t VADDR_KERNEL;

uint64_t addr_offset_higher(uint64_t p);
uint64_t addr_offset_lower(uint64_t p);
uint64_t addr_ensure_higher(uint64_t p);
uint64_t addr_ensure_lower(uint64_t p);

/* a needs to be a multiple of 2 */
uint64_t addr_align_up(uint64_t p, size_t a);
uint64_t addr_align_down(uint64_t p, size_t a);
uint64_t addr_page_align_up(uint64_t p);
uint64_t addr_page_align_down(uint64_t p);

bool ptr_is_user(void *ptr);

#endif
