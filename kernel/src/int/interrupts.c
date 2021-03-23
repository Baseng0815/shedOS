#include "interrupts.h"

#include <stdbool.h>

#include <printk.h>

__attribute__((interrupt)) void pf_handle(struct interrupt_frame *irf)
{
        printk(KMSG_LOGLEVEL_CRIT, "PAGE FAULT!!!1111\n");

        while (true);
}
