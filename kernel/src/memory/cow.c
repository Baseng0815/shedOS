#include "cow.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"

#include "../task/sched.h"
#include "vmm.h"
#include "pmm.h"

static void *kernel_copy_vaddr = (void*)0xffff810200000000UL;

void cow_copy_on_fault(void *vaddr)
{
        uint64_t *parent_entry = paging_entry_get(current_task->vmap_parent,
                                                  vaddr, false);
        if (!parent_entry) {
                printf(KMSG_LOGLEVEL_WARN, "parent doesn't have %x\n", vaddr);
                return;
        }

        /* map physical page of parent to some fixed address in kernel space */
        uintptr_t parent_physical_addr  = *parent_entry & ~0xfffUL;
        uint8_t parent_flags            = *parent_entry & 0xfff;
        paging_map(kernel_table,
                   kernel_copy_vaddr,
                   parent_physical_addr,
                   parent_flags);

        /* new page for the process */
        vmm_request_at(current_task->vmap, vaddr, 1, parent_flags | PAGING_USER);

        printf(KMSG_LOGLEVEL_CRIT, "parent physical addr is %x\n",
               parent_physical_addr);

        /* copy data from mapped page */
        memcpy(vaddr, kernel_copy_vaddr, 0x1000);
}
