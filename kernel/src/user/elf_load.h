#ifndef _ELF_LOAD_H
#define _ELF_LOAD_H

#include "../memory/paging.h"

struct task {
        struct page_table *vmmap;
};

void elf_load(uint8_t *elf_data, struct task *task);

#endif
