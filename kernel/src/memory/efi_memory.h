#ifndef _EFI_MEMORY_H
#define _EFI_MEMORY_H

#include <stdint.h>
#include <stddef.h>

struct efi_memory_descriptor {
        uint32_t    type;
        void        *paddr;
        void        *vaddr;
        uint64_t    page_count;
        uint64_t    attr;
};

struct efi_memory_map {
        void        *paddr;
        size_t      size;
        size_t      desc_size;
};

extern const char *efi_memory_strings[];

#endif
