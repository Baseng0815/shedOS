#include "cow.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"

#include "../task/sched.h"
#include "vmm.h"
#include "pmm.h"

static void *kernel_copy_vaddr = (void*)0xffff810200000000UL;

bool cow_copy_on_write(void *vaddr)
{
        uint64_t *parent_entry = paging_entry_get(current_task->vmap_parent,
                                                  vaddr, 0);
        uint8_t parent_flags = *parent_entry & 0xfff;
        if (!parent_entry ||
            !(parent_flags & PAGING_WRITABLE) ||
            !(parent_flags & PAGING_USER)) {
                return false;
        }

        /* map physical page of parent to kernel_copy_vaddr */
        uintptr_t parent_physical_addr  = *parent_entry & ~0xfffUL;
        paging_map(kernel_table, kernel_copy_vaddr, parent_physical_addr, 0);

        /* unmap old, shared, read-only page */
        paging_unmap(current_task->vmap, vaddr);

        /* new page for the process, this time with writable bit set */
        vmm_request_at(current_task->vmap, vaddr, 1, parent_flags | PAGING_WRITABLE);

        /* copy data from kernel_copy_vaddr to new page */
        memcpy(vaddr, kernel_copy_vaddr, 0x1000);

        /* unmap kernel_copy_vaddr */
        paging_unmap(kernel_table, kernel_copy_vaddr);

        return true;
}
