#include "task.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"
#include "../libk/bump_alloc.h"
#include "../memory/addrutil.h"
#include "../memory/vmm.h"
#include "elf.h"

#define HDR_OFF(off)((void*)((uintptr_t)(hdr))+(off))

void task_create(struct task **new_task, const uint8_t *elf_data)
{
        /* allocate task structure */
        struct task *task = bump_alloc(sizeof(struct task), 0);
        *new_task = task;
        memset(task, 0, sizeof(struct task));

        printf(KMSG_LOGLEVEL_INFO, "Loading elf at %x\n", elf_data);

        /* create new address space */
        task->vmap = paging_create_empty();
        paging_write_cr3(task->vmap);

        /* load elf into address space */
        const Elf64_Ehdr *hdr = (Elf64_Ehdr*)elf_data;

        const Elf64_Phdr *phdrs = HDR_OFF(hdr->e_phoff);
        for (size_t i = 0; i < hdr->e_phnum; i++) {
                if (phdrs[i].p_type == PT_LOAD) {
                        uint8_t flags = PAGING_USER;
                        /* writable */
                        if (phdrs[i].p_flags & PF_W) flags |= PAGING_WRITABLE;

                        size_t to_alloc =
                                addr_page_align_up(phdrs[i].p_memsz +
                                                   phdrs[i].p_vaddr % 0x1000);
                        vmm_request_at(task->vmap,
                                       phdrs[i].p_vaddr,
                                       to_alloc / 0x1000,
                                       flags);
                        memset(phdrs[i].p_vaddr, 0, phdrs[i].p_filesz);
                        memcpy((void*)phdrs[i].p_vaddr,
                               HDR_OFF(phdrs[i].p_offset),
                               phdrs[i].p_memsz);
                }
        }

        task->rsp = 0x7ffffffffff0UL;
        vmm_request_at(task->vmap, addr_page_align_down(task->rsp), 1,
                       PAGING_USER | PAGING_WRITABLE);
        task->rip = hdr->e_entry;

        printf(KMSG_LOGLEVEL_CRIT, "ELF loaded and task created.\n");
}
