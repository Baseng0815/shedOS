#include "apic.h"

#include "../libk/printf.h"

#define ENTRY_LAPIC                     0x0
#define ENTRY_IO_APIC                   0x1
#define ENTRY_INTERRUPT_SOURCE_OVERRIDE 0x2
#define ENTRY_NON_MASKABLE_INTERRUPT    0x4
#define ENTRY_LAPIC_ADDRESS_OVERRIDE    0x5

#define APIC_SIV_REGISTER_OFFSET        0xf0

/* to set the core count */
#include "../cpuinfo.h"

static uintptr_t lapic_base;

static size_t parse_madt_entry(struct madt_entry_header*);

void apic_initialize(struct madt *madt)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target apic.\n");

        printf(KMSG_LOGLEVEL_INFO,
               "madt at %x, lapic addr=%x, flags=%x\n",
               madt, madt->local_apic, madt->flags);

        lapic_base = madt->local_apic;
        size_t record_size = madt->hdr.length - sizeof(struct madt);

        cpuinfo.core_count = 0;
        for (size_t offset = 0; offset < record_size;) {
                struct madt_entry_header *hdr = (struct madt_entry_header*)
                        ((uintptr_t)madt + sizeof(struct madt) + offset);

                offset += parse_madt_entry(hdr);
        }

        printf(KMSG_LOGLEVEL_INFO, "%d cores detected.\n", cpuinfo.core_count);

        /*
           a rare timing issue may create spurious interrupts which need
           to be delivered. to use the local APIC, we need to configure
           the spurious interrupt vector register with a value > 32 and
           bit 8 (APIC software enable) set to 1
           */
        *(uint32_t*)(lapic_base + APIC_SIV_REGISTER_OFFSET) = 0xff;

        printf(KMSG_LOGLEVEL_OKAY, "Finished target apic.\n");
}

size_t parse_madt_entry(struct madt_entry_header *hdr)
{
        switch (hdr->entry_type) {
                case ENTRY_LAPIC: {
                        struct madt_entry_lapic *entry =
                                (struct madt_entry_lapic*)hdr;

                        cpuinfo.core_count++;
                }

                default:
                        break;
        }

        return hdr->record_length;
}
