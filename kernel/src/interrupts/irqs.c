#include "irqs.h"

#include "../libk/printf.h"

#include "pic.h"

__attribute__((interrupt)) void hpet_handle(struct interrupt_frame *frame)
{
        static int i = 1;
        if (i & 1) {
                printf(KMSG_LOGLEVEL_WARN, "tick\n");
        } else {
                printf(KMSG_LOGLEVEL_CRIT, "tock\n");
        }

        i ^= 1;

        /* EOI xddd */
        *(uint32_t*)(0xfee000b0) = 0;
}
