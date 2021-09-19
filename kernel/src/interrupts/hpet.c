#include "hpet.h"

#include <stdbool.h>

#include "../debug.h"

#include "../libk/printf.h"
#include "../memory/addrutil.h"
#include "timer.h"

/* HPET register */
#define HPET_REG_GENERAL_CAPABILITIES   0x0
#define HPET_REG_GENERAL_CONFIGURATION  0x02
#define HPET_REG_GENERAL_INT_STATUS     0x04
#define HPET_REG_MAIN_COUNTER_VALUE     0x1e

#define HPET_REG_TIMER_N_CONF_CAP(N)\
        (0x20 + 0x4 * N)

#define HPET_REG_TIMER_N_COMPARATOR(N)\
        (0x21 + 0x4 * N)

#define HPET_REG_TIMER_N_FSB(N)\
        (0x22 + 0x4 * N)

/* HPET main capabilities */
#define MAIN_CAP_REVISION_ID    0x0
#define MAIN_CAP_TIMER_COUNT    0x08
#define MAIN_CAP_QWORD_CAPABLE  0x0d
#define MAIN_CAP_LEGACY_CAPABLE 0x0f
#define MAIN_CAP_VENDOR_ID      0x10
#define MAIN_CAP_CLOCK_PERIOD   0x20


/* HPET main configuration */
#define MAIN_CONF_COUNTER_ENABLE    0x0
#define MAIN_CONF_LEGACY_ENABLE     0x1

/* HPET timer configuration and capabilities */
#define TIMER_CONF_LEVEL_TRIGGERED  0x1
#define TIMER_CONF_IRQ_ENABLE       0x2
#define TIMER_CONF_PERIODIC_ENABLE  0x3
#define TIMER_CONF_PERIODIC_CAPABLE 0x4
#define TIMER_CONF_IS_64_BIT        0x5
#define TIMER_CONF_DIRECT_ACCUM_SET 0x6
#define TIMER_CONF_FORCE_32_BIT     0x8
#define TIMER_CONF_ROUTING_CONF     0x9
#define TIMER_CONF_FSB_USE          0xe
#define TIMER_CONF_FSB_CAPABLE      0xf
#define TIMER_CONF_ROUTING_CAP      0x20

static uint64_t *hpet_mmio_regs;

void hpet_initialize(struct hpet *hpet)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing hpet...\n");

        hpet_mmio_regs = addr_ensure_higher(hpet->addr.addr);

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
        assert(main_cap | MAIN_CAP_LEGACY_CAPABLE > 0,
               "HPET not legacy capable.");

        uint32_t clock_period   = main_cap >> MAIN_CAP_CLOCK_PERIOD;
        uint8_t timer_count     = (main_cap >> MAIN_CAP_TIMER_COUNT & 0x1f) + 1;
        uint64_t main_frequency = 1000000000000000 / clock_period;

        /* configure timers (not necessary for now) */
        for (size_t i = 0; i < timer_count; i++) {
                uint64_t timer_capabilities =
                        hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(i)];

                uint32_t rc = timer_capabilities >> TIMER_CONF_ROUTING_CAP;
                printf(KMSG_LOGLEVEL_INFO,
                       "Timer %d routing capabilities %x\n", i, rc);
        }

        /* we use legacy replacement mapping so the first two timers are
           used to replace the PIT (first timer is using IRQ 0) */

        /* configure timer 0 (kernel ticks) */
        uint64_t timer0 = hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(0)];
        assert((timer0 & 1ul << TIMER_CONF_PERIODIC_CAPABLE) > 0,
               "Timer0 does not support periodic mode.");

        timer0 |= 1UL << TIMER_CONF_IRQ_ENABLE;
        timer0 |= 1UL << TIMER_CONF_PERIODIC_ENABLE;
        timer0 |= 1UL << TIMER_CONF_DIRECT_ACCUM_SET;

        hpet_mmio_regs[HPET_REG_TIMER_N_CONF_CAP(0)] = timer0;

        /* kernel ticks per second */
        hpet_mmio_regs[HPET_REG_TIMER_N_COMPARATOR(0)]
                = main_frequency / ticks_per_second;

        /* configure timer 1 (framebuffer swap) */

        /* reset timer */
        hpet_mmio_regs[HPET_REG_MAIN_COUNTER_VALUE] = 0;

        /* enable main timer */
        uint64_t main_conf = hpet_mmio_regs[HPET_REG_GENERAL_CONFIGURATION];
        main_conf |= 1UL << MAIN_CONF_COUNTER_ENABLE;
        main_conf |= 1UL << MAIN_CONF_LEGACY_ENABLE;
        hpet_mmio_regs[HPET_REG_GENERAL_CONFIGURATION] = main_conf;

        printf(KMSG_LOGLEVEL_OKAY, "Initialized hpet.\n");
}
