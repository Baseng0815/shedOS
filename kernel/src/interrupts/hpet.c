#include "hpet.h"

#include <stdbool.h>

#include "../debug.h"

#include "../libk/printf.h"
#include "../memory/paging.h"

enum hpet_register {
        HPET_REG_GENERAL_CAPABILITIES   = 0x00,
        HPET_REG_GENERAL_CONFIGURATION  = 0x02,
        HPET_REG_GENERAL_INT_STATUS     = 0x04,
        HPET_REG_MAIN_COUNTER_VALUE     = 0x1e
};

#define HPET_REG_TIMER_N_CONF_CAP(N)\
        (0x20 + 0x4 * N)

#define HPET_REG_TIMER_N_COMPARATOR(N)\
        (0x21 + 0x4 * N)

#define HPET_REG_TIMER_N_FSB(N)\
        (0x22 + 0x4 * N)

enum hpet_main_capabilities {
        REVISION_ID         = 0x0,
        TIMER_COUNT         = 0x08,
        QWORD_CAPABLE       = 0x0d,
        LEGACY_CAPABLE      = 0x0f,
        VENDOR_ID           = 0x10,
        CLOCK_PERIOD        = 0x20
};

enum hpet_main_configuration {
        COUNTER_ENABLE      = 0x0,
        LEGACY_ENABLE       = 0x1
};

enum hpet_timer_conf_cap {
        LEVEL_TRIGGERED     = 0x1,
        IRQ_ENABLE          = 0x2,
        PERIODIC_ENABLE     = 0x3,
        PERIODIC_CAPABLE    = 0x4,
        IS_64_BIT           = 0x5,
        DIRECT_ACCUM_SET    = 0x6,
        FORCE_32_BIT        = 0x8,
        ROUTING_CONF        = 0x9,
        FSB_USE             = 0xe,
        FSB_CAPABLE         = 0xf,
        ROUTING_CAP         = 0x20
};

static uint64_t *hpet_mmio_regs;

void hpet_initialize(struct hpet *hpet)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing hpet...\n");

        hpet_mmio_regs = vaddr_ensure_higher(hpet->addr.addr);

        printf(KMSG_LOGLEVEL_INFO,
               "HPET present with pci vendor id %d, addr %x, "
               "HPET number %d, min main counter clock_tick %d, "
               "comparator count %d, legacy IRQ capable %d\n",
               hpet->pci_vendor_id, hpet->addr.addr,
               hpet->hpet_number, hpet->min_clock_ticks,
               hpet->comparator_count + 1, hpet->legacy_replacement);

        /* get main capabilities */
        uint64_t main_cap =
                hpet_mmio_regs[HPET_REG_GENERAL_CAPABILITIES];

        /* we use legacy replacement mapping for now */
        assert(main_cap | LEGACY_CAPABLE > 0,
               "HPET not legacy capable.");

        uint32_t clock_period   = main_cap >> CLOCK_PERIOD & 0xffffffff;
        uint8_t timer_count     = (main_cap >> TIMER_COUNT & 0x1f) + 1;
        uint64_t main_frequency = 1000000000000000 / clock_period;

        /* configure timers (not necessary for now) */
        for (size_t i = 0; i < timer_count; i++) {
                uint64_t timer_capabilities =
                        hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(i)];

                printf(KMSG_LOGLEVEL_INFO,
                       "Timer %d routing capabilities %x\n", i,
                       timer_capabilities >> ROUTING_CAP & 0xffffffff);
        }

        /* we use legacy replacement mapping so the first two timers are
           used to replace the PIT (first timer is using IRQ 0) */

        /* configure timer 0 (kernel ticks) */
        uint64_t timer0 = hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(0)];
        assert((timer0 & 1ul << PERIODIC_CAPABLE) > 0,
               "Timer0 does not support periodic mode.");

        timer0 |= 1ul << IRQ_ENABLE;
        timer0 |= 1ul << PERIODIC_ENABLE;
        timer0 |= 1ul << DIRECT_ACCUM_SET;

        hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(0)] = timer0;

        /* 100000 kernel ticks per second */
        hpet_mmio_regs[HPET_REG_TIMER_N_COMPARATOR(0)]
                = main_frequency / 100000;

        /* configure timer 1 (framebuffer swap) */

        /* reset timer */
        hpet_mmio_regs[HPET_REG_MAIN_COUNTER_VALUE] = 0;

        /* enable main timer */
        uint64_t main_conf = hpet_mmio_regs[HPET_REG_GENERAL_CONFIGURATION];
        main_conf |= 1ul << COUNTER_ENABLE;
        main_conf |= 1ul << LEGACY_ENABLE;
        hpet_mmio_regs[HPET_REG_GENERAL_CONFIGURATION] = main_conf;

        printf(KMSG_LOGLEVEL_OKAY, "Initialized hpet.\n");
}
