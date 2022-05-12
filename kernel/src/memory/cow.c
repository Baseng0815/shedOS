#include "cow.h"

#include "../libk/printf.h"
#include "../libk/memutil.h"

#include "../task/sched.h"
#include "vmm.h"
#include "pmm.h"

#include "../memory/addrutil.h"

static void *kernel_copy_vaddr = (void*)0xffff810200000000UL;

bool cow_copy_on_write(void *vaddr)
{
        uint64_t offending_page = (uint64_t)vaddr & ~0xfffUL;

        /* map physical page to kernel_copy_vaddr */
        uint64_t entry = paging_get(current_task->vmap, (void*)offending_page);
        if (!(entry & PAGING_PRESENT))
                return false;

        paging_map(kernel_table,
                   kernel_copy_vaddr,
                   (void*)(entry & ~0xfffUL),
                   0);

        /* unmap old read-only page */
        paging_unmap(current_task->vmap, (void*)offending_page);

        /* new page for the process, this time with writable bit set */
        vmm_request_at(current_task->vmap, (void*)offending_page, 1,
                       (entry & 0xfff) | PAGING_WRITABLE);

        /* copy data from kernel_copy_vaddr to new page */
        memcpy((void*)offending_page, kernel_copy_vaddr, 0x1000);

        /* unmap kernel_copy_vaddr */
        paging_unmap(kernel_table, kernel_copy_vaddr);

        return true;
}
