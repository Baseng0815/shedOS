#include "irqs.h"

#include "../libk/printf.h"

#include "apic.h"
#include "timer.h"

__attribute__((interrupt)) void hpet_handle(struct interrupt_frame *frame)
{
        timer_tick();

        /* printf(KMSG_LOGLEVEL_CRIT, "xdd\n"); */

        apic_send_eoi();
}
