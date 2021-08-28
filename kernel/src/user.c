#include "user.h"

#include "memory/vmm.h"
#include "libk/printf.h"

void user_jump()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target user.\n");

        const char *msg = "This is a string printed from ring 3 using a "
                "system call interrupt!\n";

        asm volatile("movq %0, %%rdi;"
                     "int $0x80;"
                     : : "D" (msg));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target user.\n");
}
