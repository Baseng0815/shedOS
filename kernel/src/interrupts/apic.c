#include "apic.h"

#include "../libk/printf.h"

#define ENTRY_LAPIC                     0x0
#define ENTRY_IO_APIC                   0x1
#define ENTRY_INTERRUPT_SOURCE_OVERRIDE 0x2
#define ENTRY_NON_MASKABLE_INTERRUPT    0x4
#define ENTRY_LAPIC_ADDRESS_OVERRIDE    0x5

static int core_count;

static size_t parse_madt_entry(struct madt_entry_header*);

void apic_initialize(struct madt *madt)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target apic.\n");

        printf(KMSG_LOGLEVEL_INFO,
               "madt at %x, lapic addr=%x, flags=%x\n",
               madt, madt->local_apic, madt->flags);

        size_t record_size = madt->hdr.length - sizeof(struct madt);
        printf(KMSG_LOGLEVEL_NONE,
               "|-> madt length=%x, struct size=%x => %x bytes of records\n",
               madt->hdr.length, sizeof(struct madt), record_size);

        core_count = 0;
        for (size_t offset = 0; offset < record_size;) {
                struct madt_entry_header *hdr = (struct madt_entry_header*)
                        ((uintptr_t)madt + sizeof(struct madt) + offset);

                offset += parse_madt_entry(hdr);
        }

        printf(KMSG_LOGLEVEL_INFO, "%d cores detected.\n", core_count);

        printf(KMSG_LOGLEVEL_SUCC, "Finished target apic.\n");
}

static size_t parse_madt_entry(struct madt_entry_header *hdr)
{
        switch (hdr->entry_type) {
                case ENTRY_LAPIC: {
                        struct madt_entry_lapic *entry =
                                (struct madt_entry_lapic*)hdr;
                        printf(KMSG_LOGLEVEL_NONE,
                               "|---> lapic %d with processor_id=%d, "
                               "flags=%x\n",
                               entry->apic_id, entry->acpi_processor_id,
                               entry->flags);

                        core_count++;
                }
                break;
                case ENTRY_IO_APIC: {
                        struct madt_entry_io_apic *entry =
                                (struct madt_entry_io_apic*)hdr;
                        printf(KMSG_LOGLEVEL_NONE,
                               "|---> i/o apic %d, addr=%x, gsib=%x\n",
                               entry->io_apic_id, entry->io_apic_addr,
                               entry->gsib);
                        break;
                }
                case ENTRY_INTERRUPT_SOURCE_OVERRIDE: {
                        struct madt_entry_iso *entry =
                                (struct madt_entry_iso*)hdr;
                        printf(KMSG_LOGLEVEL_NONE,
                               "|---> iso with bus source=%d, irq_source=%d, "
                               "gsi=%d, flags=%x\n",
                               entry->bus_source, entry->irq_source,
                               entry->gsi, entry->flags);
                        break;
                }
                case ENTRY_NON_MASKABLE_INTERRUPT: {
                        struct madt_entry_nmi *entry =
                                (struct madt_entry_nmi*)hdr;
                        printf(KMSG_LOGLEVEL_NONE,
                               "|---> nmi with acpi id=%d, flags=%x, lint=%d\n",
                               entry->acpi_processor_id, entry->flags,
                               entry->lint);
                        break;
                }
                case ENTRY_LAPIC_ADDRESS_OVERRIDE: {
                        struct madt_entry_laao *entry =
                                (struct madt_entry_laao*)hdr;
                        printf(KMSG_LOGLEVEL_NONE,
                               "|---> laao with lapic addr=%x\n",
                               entry->lapic_addr);
                        break;
                }

                default: {
                        printf(KMSG_LOGLEVEL_WARN,
                               "|---> madt entry of type %d not recognized\n",
                               hdr->entry_type);
                        break;
                }
        }

        return hdr->record_length;
}
