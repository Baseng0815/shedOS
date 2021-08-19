#include "user.h"

#include "memory/vmm.h"
#include "libk/printf.h"

void user_jump()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target user.\n");

        /* uint8_t *addr = (uint8_t*)0xffff999910000000; */
        /* vmm_request_at(kernel_table, addr, 1, PAGING_WRITABLE); */
        /* *addr = 31; */

        printf(KMSG_LOGLEVEL_OKAY, "Finished target user.\n");
}
