#include "debug.h"

#include "libk/printf.h"

void kernel_panic(const char *message, const char *file, int line)
{
        printf(KMSG_LOGLEVEL_CRIT, "KERNEL PANIC: %s at %s:%d\n",
               message, file, line);

        for (;;) {
                asm volatile("hlt");
        }
}
