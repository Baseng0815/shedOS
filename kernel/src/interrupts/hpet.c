#include "hpet.h"

#include <stdbool.h>

#include "../debug.h"

#include "../libk/printf.h"

enum register_offset {
        HPET_REG_GENERAL_CAPABILITIES   = 0x00,
        HPET_REG_GENERAL_CONFIGURATION  = 0x10,
        HPET_REG_GENERAL_INT_STATUS     = 0x20,
        HPET_REG_MAIN_COUNTER_VALUE     = 0xf0
};

#define HPET_REG_TIMER_N_CONF_CAP(N)\
        (0x100 + 0x20 * N)

#define HPET_REG_TIMER_N_COMPARATOR(N)\
        (0x108 + 0x20 * N)

#define HPET_REG_TIMER_N_FSB_IRQ_ROUTE(N)\
        (0x110 + 0x20 * N)

struct hpet_main_capabilities {
        uint64_t revision_id        : 8;
        uint64_t timer_count        : 5;
        uint64_t is_64_capable      : 1;
        uint64_t reserved0          : 1;
        uint64_t legacy_rt_capable  : 1;    /* legacy replacement mapping   */
        uint64_t vendor_id          : 16;
        uint64_t tick_period        : 32;   /* in femtoseconds              */
} __attribute__((packed));

struct hpet_main_configuration {
        uint64_t main_counter_enable    : 1;
        uint64_t legacy_rt_enable       : 1;
        uint64_t reserved0              : 62;
} __attribute__((packed));

struct hpet_timer_conf_cap {
        uint64_t reserved0              : 1;
        uint64_t level_IRQs             : 1;    /* r/w  */
        uint64_t IRQ_enable             : 1;    /* r/w  */
        uint64_t periodic_mode          : 1;    /* r/w  */
        uint64_t periodic_available     : 1;    /* r    */
        uint64_t size_is_64             : 1;    /* r    */
        uint64_t allow_direct_accum_set : 1;    /* r/w  */
        uint64_t reserved1              : 1;
        uint64_t force_32               : 1;    /* r/w  */
        uint64_t io_apic_routing        : 5;    /* r/w  */
        uint64_t use_FSB_mapping        : 1;    /* r/w  */
        uint64_t FSB_mapping_available  : 1;    /* r    */
        uint64_t reserved2              : 16;
        uint64_t IRQ_routing_capability : 32;   /* r/w  */
} __attribute__((packed));

static uintptr_t hpet_addr;

void main_conf_write(struct hpet_main_configuration);
struct hpet_timer_conf_cap timer_conf_read(uint8_t);
void timer_conf_write(uint8_t, struct hpet_timer_conf_cap);

void timer_comparator_write(uint8_t, uint64_t);

void hpet_initialize(struct hpet *hpet)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing hpet...\n");

        hpet_addr = hpet->addr.addr;

        printf(KMSG_LOGLEVEL_INFO,
               "HPET present with pci vendor id %d, addr %x, "
               "HPET number %d, min main counter clock_tick %d, "
               "comparator count %d, legacy IRQ capable %d\n",
               hpet->pci_vendor_id, hpet->addr.addr,
               hpet->hpet_number, hpet->min_clock_ticks,
               hpet->comparator_count, hpet->legacy_replacement);

        /* get main capabilities */

        /* and the mmio administrator has spoken:
           thou shalt only read and write at aligned addresses
           => always read/write whole struct and don't keep pointers
           */
        struct hpet_main_capabilities main_capabilities
                = *(struct hpet_main_capabilities*)
                ((uintptr_t)hpet->addr.addr + HPET_REG_GENERAL_CAPABILITIES);

        /* we use legacy replacement mapping for now */
        assert(main_capabilities.legacy_rt_capable > 0,
               "HPET not legacy capable.");

        uint64_t main_frequency =
                1000000000000000 / (uint64_t)main_capabilities.tick_period;

        printf(KMSG_LOGLEVEL_NONE,
               "|-> revision_id=%d, timer_count=%d, 64 bit/legacy rt capable "
               "%d/%d, vendor_id=%d, tick_period=%d, frequency=%d\n",
               main_capabilities.revision_id, main_capabilities.timer_count,
               main_capabilities.is_64_capable,
               main_capabilities.legacy_rt_capable,
               main_capabilities.vendor_id, main_capabilities.tick_period,
               main_frequency);

        printf(KMSG_LOGLEVEL_NONE, "|-> frequency %d\n", main_frequency);

        /* configure timers (not necessary for now) */
        for (size_t i = 0; i < main_capabilities.timer_count; i++) {
                struct hpet_timer_conf_cap timer_capabilities =
                        *(struct hpet_timer_conf_cap*)
                        ((uintptr_t)hpet->addr.addr +
                         HPET_REG_TIMER_N_CONF_CAP(i));

                printf(KMSG_LOGLEVEL_INFO,
                       "Timer %d routing capabilities %x\n",
                       i, timer_capabilities.IRQ_routing_capability);
        }

        /* we use legacy replacement mapping so the first two timers are
           used to replace the PIT (first timer is using IRQ 0) */

        /* configure timer 0 */
        struct hpet_timer_conf_cap timer0 = timer_conf_read(0);
        assert(timer0.periodic_available > 0,
               "Timer0 does not support periodic mode.");

        timer0.IRQ_enable               = 1;
        timer0.periodic_mode            = 1;
        timer0.allow_direct_accum_set   = 1;

        timer_conf_write(0, timer0);
        timer_comparator_write(0, main_frequency);

        /* reset timer */
        *(uint64_t*)(hpet_addr + HPET_REG_MAIN_COUNTER_VALUE) = 0;

        /* enable main timer */
        struct hpet_main_configuration main_configuration = {
                .main_counter_enable    = 1,
                .legacy_rt_enable       = 1
        };

        main_conf_write(main_configuration);

        printf(KMSG_LOGLEVEL_OKAY, "Initialized hpet.\n");
}

void main_conf_write(struct hpet_main_configuration main_conf)
{
        *(struct hpet_main_configuration*)
                (hpet_addr + HPET_REG_GENERAL_CONFIGURATION) =
                main_conf;
}

struct hpet_timer_conf_cap timer_conf_read(uint8_t timer)
{
        struct hpet_timer_conf_cap timer_conf_cap =
                *(struct hpet_timer_conf_cap*)
                (hpet_addr + HPET_REG_TIMER_N_CONF_CAP(timer));

        return timer_conf_cap;
}

void timer_conf_write(uint8_t timer,
                      struct hpet_timer_conf_cap timer_conf_cap)
{
        *(struct hpet_timer_conf_cap*)
                (hpet_addr + HPET_REG_TIMER_N_CONF_CAP(timer)) =
                timer_conf_cap;
}

void timer_comparator_write(uint8_t timer,
                            uint64_t value)
{
        *(uint64_t*)(hpet_addr + HPET_REG_TIMER_N_COMPARATOR(timer)) = value;
}
