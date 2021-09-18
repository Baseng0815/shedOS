#include "task.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"
#include "../libk/kmalloc.h"
#include "../memory/addrutil.h"
#include "../memory/vmm.h"
#include "elf.h"

#define HDR_OFF(off)((void*)((uintptr_t)(hdr))+(off))

void create_task(struct task **new_task, uint8_t *elf_data)
{
        /* allocate task structure */
        *new_task = kmalloc(sizeof(struct task));
        memset(*new_task, 0, sizeof(struct task));

        printf(KMSG_LOGLEVEL_INFO, "Loading elf at %x\n", elf_data);

        asm volatile("cli");
        /* create new address space */''
        paging_copy_table(kernel_table, &(*new_task)->vmap);
        paging_write_cr3((*new_task)->vmap);

        /* load elf into address space */
        const Elf64_Ehdr *hdr = (Elf64_Ehdr*)elf_data;

        const Elf64_Phdr *phdrs = HDR_OFF(hdr->e_phoff);
        for (size_t i = 0; i < hdr->e_phnum; i++) {
                if (phdrs[i].p_type == PT_LOAD) {
                        uint8_t flags = 0;
                        /* writable */
                        if (phdrs[i].p_flags & PF_W) flags |= PAGING_WRITABLE;

                        size_t to_alloc =
                                addr_page_align_up(phdrs[i].p_memsz +
                                                   phdrs[i].p_vaddr % 0x1000);
                        vmm_request_at((*new_task)->vmap,
                                       phdrs[i].p_vaddr,
                                       to_alloc / 0x1000,
                                       flags);
                        memset(phdrs[i].p_vaddr, 0, phdrs[i].p_filesz);
                        memcpy((void*)phdrs[i].p_vaddr,
                               HDR_OFF(phdrs[i].p_offset),
                               phdrs[i].p_memsz);
                }
        }

        /* we still need a timer system to make the scheduler work */

        /* jump to entry point */
        int(*elf_entry)(void) = (int(*)(void))hdr->e_entry;
        printf(KMSG_LOGLEVEL_INFO, "Entry point %x\n", hdr->e_entry);
        int result = elf_entry();
        printf(KMSG_LOGLEVEL_WARN, "ELF result: %d\n", result);

        printf(KMSG_LOGLEVEL_OKAY, "ELF finished.\n");
}