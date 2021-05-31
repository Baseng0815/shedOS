#include "apic.h"

#include "../libk/printf.h"
#include "../debug.h"
#include "../io.h"
#include "../memory/paging.h"

#include "pic.h"

#define IOREDTBL_OFFSET(N) (0x10 + N * 2)

/* to set the core count */
#include "../cpuinfo.h"

enum lapic_register {
        LAPIC_REG_ID        = 0x08,
        LAPIC_REG_VERSION   = 0x0c,
        LAPIC_REG_TPR       = 0x20,
        LAPIC_REG_EOI       = 0x2c,
        LAPIC_REG_SIVR      = 0x3c
};

enum delivery_mode {
        FIXED           = 0x000,
        LOWEST_PRIORITY = 0x001,
        SMI             = 0x010,
        NMI             = 0x100,
        INIT            = 0x101,
        EXTINT          = 0x111
};

enum destination_mode {
        PHYSICAL    = 0x0,
        LOGICAL     = 0x1
};

enum pin_polarity {
        ACTIVE_HIGH = 0x0,
        ACTIVE_LOW  = 0x1
};

enum trigger_mode {
        EDGE    = 0x0,
        LEVEL   = 0x1,
};

/* there are 24 IOREDTBL (I/O redirection tables), one for each IRQ */
struct ioredtbl {
        uint64_t vector             : 8;
        uint64_t delivery_mode      : 3;
        uint64_t destination_mode   : 1;
        uint64_t delivery_status    : 1;
        uint64_t pin_polarity       : 1;
        uint64_t remote_irr         : 1;
        uint64_t trigger_mode       : 1;
        uint64_t mask               : 1;
        uint64_t reserved0          : 39;
        uint64_t destination        : 8;
};

size_t                      ioapic_count = 0;
struct madt_entry_ioapic    ioapics[8];

size_t                      iso_count = 0;
struct madt_entry_iso       isos[24];

static uint32_t             *lapic_mmio_regs;

static size_t parse_madt_entry(const struct madt_entry_header*);

/* IOAPIC registers are mmio. the IOREGSEL register contains the reg offset
   to read/write to and the IOWIN register contains the actual data */
void        ioapic_register_write(uint32_t ioapic_id,
                                  uint8_t offset,
                                  uint32_t val);

uint32_t    ioapic_register_read(uint32_t ioapic_id, uint8_t offset);

void        ioapic_ioredtbl_write(uint32_t ioapic_id,
                                  uint8_t index,
                                  struct ioredtbl tbl);

void apic_initialize(const struct madt *madt)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target apic.\n");

        lapic_mmio_regs = vaddr_ensure_higher(madt->local_apic);

        printf(KMSG_LOGLEVEL_INFO,
               "madt at %a, lapic addr=%a, flags=%x\n",
               madt, lapic_mmio_regs, madt->flags);

        /* initialize and disable PIC, we won't need it */
        pic_initialize();

        /* parse MADT, i.e. extract local and I/O APICs and also
           interrupt source overrides */
        size_t record_size = madt->hdr.length - sizeof(struct madt);

        cpuinfo.core_count = 0;
        for (size_t offset = 0; offset < record_size;) {
                struct madt_entry_header *hdr = (struct madt_entry_header*)
                        ((uintptr_t)madt + sizeof(struct madt) + offset);

                offset += parse_madt_entry(hdr);
        }

        assert(ioapic_count > 0, "No I/O APICs were detected");

        /* get BSP (bootstrap processor) lapic id */
        uint32_t bsp_lapic_id = lapic_mmio_regs[LAPIC_REG_ID];
        printf(KMSG_LOGLEVEL_INFO,
               "%d cores detected. BSP lapic id=%d\n",
               cpuinfo.core_count, bsp_lapic_id);

        /* set up redirection entries (only for one ioapic right now) */
        for (size_t i = 0; i < iso_count; i++) {
                struct madt_entry_iso iso = isos[i];

                struct ioredtbl tbl;
                tbl.delivery_mode = FIXED;
                tbl.destination_mode = 0;
                tbl.pin_polarity =
                        (iso.flags & 0x2 > 0) ? ACTIVE_LOW : ACTIVE_HIGH;
                tbl.trigger_mode =
                        (iso.flags & 0x8 > 0) ? LEVEL : EDGE;
                tbl.mask = 0;
                tbl.destination = bsp_lapic_id;
                tbl.vector = iso.gsi + 0x20;

                printf(KMSG_LOGLEVEL_INFO,
                       "Setting up redirection entry in IOAPIC %d with IRQ "
                       "source %d and GSI %d (flags %x)\n",
                       bsp_lapic_id, iso.irq_source, iso.gsi, iso.flags);

                ioapic_ioredtbl_write(ioapics[0].ioapic_id, iso.gsi, tbl);
        }

        /* set spurious interrupt vector to 255 */
        lapic_mmio_regs[LAPIC_REG_SIVR] |= 0xff;

        /* enable APIC */
        lapic_mmio_regs[LAPIC_REG_SIVR] |= 0x100;

        printf(KMSG_LOGLEVEL_OKAY, "Finished target apic.\n");
}

void apic_send_eoi()
{
        lapic_mmio_regs[LAPIC_REG_EOI] = 0;
}

size_t parse_madt_entry(const struct madt_entry_header *hdr)
{
        switch (hdr->entry_type) {
                case ENTRY_LAPIC: {
                        struct madt_entry_lapic *entry =
                                (struct madt_entry_lapic*)hdr;

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
                        *(uint32_t*)vaddr_ensure_higher(
                                ioapics[i].ioapic_addr + 0x00) = offset;
                        /* 32-bit IOWIN */
                        *(uint32_t*)vaddr_ensure_higher(
                                ioapics[i].ioapic_addr + 0x10) = val;
                }
        }
}

uint32_t ioapic_register_read(uint32_t ioapic_id,
                              uint8_t offset)
{
        for (size_t i = 0; i < ioapic_count; i++) {
                if (ioapics[i].ioapic_id == ioapic_id) {
                        /* IOREGSEL */
                        *(uint32_t*)vaddr_ensure_higher(
                                ioapics[i].ioapic_addr + 0x00) = offset;
                        /* 32-bit IOWIN */
                        return *(uint32_t*)vaddr_ensure_higher(
                                ioapics[i].ioapic_addr + 0x10);
                }
        }
}

void ioapic_ioredtbl_write(uint32_t ioapic_id,
                           uint8_t index,
                           struct ioredtbl tbl)
{
        uint64_t table = *((uint64_t*)&tbl);

        ioapic_register_write(ioapic_id,
                              vaddr_ensure_higher(IOREDTBL_OFFSET(index) + 0),
                              (uint32_t)(table >> 0));

        ioapic_register_write(ioapic_id,
                              vaddr_ensure_higher(IOREDTBL_OFFSET(index) + 1),
                              (uint32_t)(table >> 32));
}
