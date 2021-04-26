#include "timer.h"

#include "../sdt/sdt.h"
#include "hpet.h"

#include "../libk/printf.h"

void timer_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target timer.\n");

        if (hpet) {
                hpet_initialize(hpet);
        }

        /* set up interrupts */

        printf(KMSG_LOGLEVEL_OKAY, "Finished target timer.\n");
}
