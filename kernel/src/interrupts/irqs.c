#include "irqs.h"

#include "../libk/printf.h"

__attribute__((interrupt)) void hpet_handle(struct interrupt_frame *frame)
{
        printf(KMSG_LOGLEVEL_WARN, "HPET INTERRUPT!\n");
}
