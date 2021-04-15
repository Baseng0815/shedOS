#include "timer.h"

#include "../sdt/sdt.h"

#include "../libk/printf.h"

void timer_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target timer.\n");

        if (hpet) {
                printf(KMSG_LOGLEVEL_INFO,
                       "HPET present with pci vendor id %d, addr %x, "
                       "HPET number %d, min main counter clock_tick %d, "
                       "comparator count %d, legacy IRQ capable %d\n",
                       hpet->pci_vendor_id, hpet->addr.addr,
                       hpet->hpet_number, hpet->min_clock_ticks,
                       hpet->comparator_count, hpet->legacy_replacement);
        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target timer.\n");
}
