#include "user.h"

#include "memory/vmm.h"
#include "libk/printf.h"

void user_jump()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target user.\n");

        int i = 1;
        for (;;) {
                if (i)
                        printf(KMSG_LOGLEVEL_CRIT, "this is ring 3\n");
                else
                        printf(KMSG_LOGLEVEL_WARN, "UwU\n");

                i ^= 1;
        }

        /* uint8_t *addr = (uint8_t*)0xffff999910000000; */
        /* vmm_request_at(kernel_table, addr, 1, PAGING_WRITABLE); */
        /* *addr = 31; */

        printf(KMSG_LOGLEVEL_OKAY, "Finished target user.\n");
}
