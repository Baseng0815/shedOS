#include "hpet.h"

#include <stdbool.h>

#include "../libk/printf.h"

#define HPET_REG_GENERAL_CAPABILITIES   0x00
#define HPET_REG_GENERAL_CONFIGURATION  0x10

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

struct hpet_timer_capabilities {
        uint64_t reserved0              : 1;
        uint64_t level_IRQs             : 1;    /* r/w  */
        uint64_t IRQ_enable             : 1;    /* r/w  */
        uint64_t periodic_mode          : 1;    /* r/w  */
        uint64_t periodic_available     : 1;    /* r    */
        uint64_t size_is_64             : 1;    /* r    */
        uint64_t val_set_cnf            : 1;
        uint64_t reserved1              : 1;
        uint64_t force_32               : 1;    /* r/w  */
        uint64_t io_apic_routing        : 5;    /* r/w  */
        uint64_t use_FSB_mapping        : 1;    /* r/w  */
        uint64_t FSB_mapping_available  : 1;    /* r    */
        uint64_t reserved2              : 16;
        uint64_t IRQ_routing_capability : 32;   /* r/w  */
} __attribute__((packed));

void hpet_initialize(struct hpet *hpet)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing hpet...\n");

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

        uint64_t main_frequency =
                (uint64_t)1e15 / (uint64_t)main_capabilities.tick_period;

        printf(KMSG_LOGLEVEL_NONE,
               "|-> revision_id=%d, timer_count=%d, 64 bit/legacy rt capable "
               "%d/%d, vendor_id=%d, tick_period=%d, frequency=%d\n",
               main_capabilities.revision_id, main_capabilities.timer_count,
               main_capabilities.is_64_capable,
               main_capabilities.legacy_rt_capable,
               main_capabilities.vendor_id, main_capabilities.tick_period,
               main_frequency);

        printf(KMSG_LOGLEVEL_NONE, "|-> frequency %d\n", main_frequency);

        /* configure timers */
        struct hpet_timer_capabilities timer_capabilities[32];
        for (size_t i = 0; i < main_capabilities.timer_count; i++) {
                timer_capabilities[i] = *(struct hpet_timer_capabilities*)
                        ((uintptr_t)hpet->addr.addr +
                         HPET_REG_TIMER_N_CONF_CAP(i));

                printf(KMSG_LOGLEVEL_INFO,
                       "Timer %d routing capabilities %x\n",
                       i, timer_capabilities[i].IRQ_routing_capability);
        }

        /* enable main timer */
        struct hpet_main_configuration main_configuration = {
                .main_counter_enable    = 1,
                .legacy_rt_enable       = 0,
        };

        *(struct hpet_main_configuration*)
                ((uintptr_t)hpet->addr.addr + HPET_REG_GENERAL_CONFIGURATION) =
                main_configuration;

        printf(KMSG_LOGLEVEL_OKAY, "Initialized hpet.\n");
}
