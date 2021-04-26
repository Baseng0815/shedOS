#include "apic.h"

#include "../libk/printf.h"
#include "../debug.h"
#include "../io.h"

#define IOREDTBL_OFFSET(N) (0x10 + N * 2)

/* to set the core count */
#include "../cpuinfo.h"

enum lapic_register {
        lapic_id        = 0x20,
        lapic_version   = 0x30,
        tpr             = 0x80,
        eoi             = 0xb0,
        sivr            = 0xf0
};

enum delivery_mode {
        fixed           = 0x000,
        lowest_priority = 0x001,
        smi             = 0x010,
        nmi             = 0x100,
        init            = 0x101,
        extint          = 0x111
};

enum destination_mode {
        physical    = 0x0,
        logical     = 0x1
};

enum pin_polarity {
        active_high = 0x0,
        active_low  = 0x1
};

enum trigger_mode {
        edge    = 0x0,
        level   = 0x1,
};

/* there are 24 IOREDTBL (I/O redirection tables), one for each interrupt */
struct ioredtbl {
        uint64_t vector             : 8;
        uint64_t delivery_mode      : 3;
        uint64_t destination_mode   : 1;
        uint64_t delivery_status    : 1;
        uint64_t pin_polarity       : 1;
        uint64_t remote_irr         : 1;
        uint64_t trigger_mode       : 1;
        uint64_t mask               : 1;
        uint64_t reserved0          : 40;
        uint64_t destination        : 8;
};

size_t                      ioapic_count = 0;
struct madt_entry_ioapic    ioapics[8];

size_t                      iso_count = 0;
struct madt_entry_iso       isos[24];

static void disable_pic();

static size_t parse_madt_entry(const struct madt_entry_header*);

/* IOAPIC registers are mmio. the IOREGSEL register contains the reg offset
   to read/write to and the IOWIN register contains the actual data */
void        ioapic_register_write(uint32_t, uint8_t, uint32_t);
void        ioapic_ioredtbl_write(uint32_t, uint8_t, struct ioredtbl*);
uint32_t    ioapic_register_read(uint32_t, uint8_t);

void apic_initialize(const struct madt *madt)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target apic.\n");

        printf(KMSG_LOGLEVEL_INFO,
               "madt at %x, lapic addr=%x, flags=%x\n",
               madt, madt->local_apic, madt->flags);

        /* disable PIC, we won't need it */
        printf(KMSG_LOGLEVEL_INFO, "Disabling pic...\n");
        disable_pic();

        /* parse MADT, i.e. extract local and I/O APICs and also
           interrupt source overrides */
        size_t record_size = madt->hdr.length - sizeof(struct madt);

        cpuinfo.core_count = 0;
        for (size_t offset = 0; offset < record_size;) {
                struct madt_entry_header *hdr = (struct madt_entry_header*)
                        ((uintptr_t)madt + sizeof(struct madt) + offset);

                offset += parse_madt_entry(hdr);
        }

        assert(ioapic_count > 0, "No I/O APICs were detected.");

        /* get BSP (bootstrap processor) lapic id */
        uint32_t bsp_lapic_id = *(uint32_t*)(madt->local_apic + lapic_id);
        printf(KMSG_LOGLEVEL_INFO,
               "%d cores detected. BSP lapic id=%d\n",
               cpuinfo.core_count, bsp_lapic_id);

        /* set spurious interrupt vector to 255 and enable bit of apic */
        *((uint32_t*)madt->local_apic + sivr ) |= 0x1ff;

        /* set up redirection entries (only for one ioapic) */
        struct madt_entry_ioapic ioapic = ioapics[0];
        for (size_t i = 0; i < iso_count; i++) {
                struct madt_entry_iso iso = isos[i];

                struct ioredtbl tbl;
                tbl.delivery_mode = fixed;
                tbl.destination_mode = 0;
                tbl.pin_polarity =
                        (iso.flags & 0x2 > 0) ? active_low : active_high;
                tbl.trigger_mode =
                        (iso.flags & 0x8 > 8) ? level : edge;
                tbl.mask = 0;
                tbl.destination = bsp_lapic_id;

                /* system timer */
                switch (iso.irq_source) {
                        case 0:
                                tbl.vector = 0x22;
                                break;

                        default:
                                tbl.vector = 0x20;
                                break;
                }

                printf(KMSG_LOGLEVEL_INFO,
                       "Setting up redirection entry in IOAPIC %d with IRQ "
                       "source %d and GSI %d (flags %x)\n",
                       bsp_lapic_id, iso.irq_source, iso.gsi, iso.flags);

                ioapic_ioredtbl_write(bsp_lapic_id, iso.gsi, &tbl);
        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target apic.\n");
}

void disable_pic()
{
        io_outb(0xa1, 0xff); /* PIC1 data */
        io_wait();
        io_outb(0x21, 0xff); /* PIC2 data */
        io_wait();
}

size_t parse_madt_entry(const struct madt_entry_header *hdr)
{
        switch (hdr->entry_type) {
                case ENTRY_LAPIC: {
                        struct madt_entry_lapic *entry =
                                (struct madt_entry_lapic*)hdr;

                        /* printf(KMSG_LOGLEVEL_INFO, */
                        /*        "Found local APIC with id %d and pr_id %d\n", */
                        /*        entry->apic_id, entry->acpi_processor_id); */

                        cpuinfo.core_count++;
                        break;
                }

                case ENTRY_IOAPIC:
                        ioapics[ioapic_count++] =
                                *(struct madt_entry_ioapic*)hdr;

                        break;

                case ENTRY_ISO: {
                        isos[iso_count++] =
                                *(struct madt_entry_iso*)hdr;

                        break;
                }


                default:
                        break;
        }

        return hdr->record_length;
}

void ioapic_register_write(uint32_t ioapic_id,
                           uint8_t offset,
                           uint32_t val)
{
        for (size_t i = 0; i < ioapic_count; i++) {
                if (ioapics[i].ioapic_id == ioapic_id) {
                        /* IOREGSEL */
                        *(uint32_t*)(ioapics[i].ioapic_addr + 0x00) = offset;
                        /* 32-bit IOWIN */
                        *(uint32_t*)(ioapics[i].ioapic_addr + 0x10) = val;
                }
        }
}

void ioapic_ioredtbl_write(uint32_t ioapic_id,
                           uint8_t index,
                           struct ioredtbl *tbl)
{
        uint64_t table = *((uint64_t*)tbl);
        ioapic_register_write(ioapic_id,
                              IOREDTBL_OFFSET(index) + 0,
                              (uint32_t)(table & 0x00000000ffffffff));

        ioapic_register_write(ioapic_id,
                              IOREDTBL_OFFSET(index) + 1,
                              (uint32_t)(table & 0xffffffff00000000));
}

uint32_t ioapic_register_read(uint32_t ioapic_id,
                              uint8_t offset)
{
        for (size_t i = 0; i < ioapic_count; i++) {
                if (ioapics[i].ioapic_id == ioapic_id) {
                        /* IOREGSEL */
                        *(uint32_t*)(ioapics[i].ioapic_addr + 0x00) = offset;
                        /* 32-bit IOWIN */
                        return *(uint32_t*)(ioapics[i].ioapic_addr + 0x10);
                }
        }
}
