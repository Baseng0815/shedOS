#ifndef _SDT_STRUCTS_H
#define _SDT_STRUCTS_H

#include <stdint.h>

/* all ACPI system descriptor tables start with this header */
struct sdt_header {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
} __attribute__((packed));

struct rsdp {
        char signature[8]; /* "RSD PTR " */
        uint8_t checksum;
        char oem_id[6];
        uint8_t revision;
        uint32_t rsdt_addr;
        /* those fields are only valid if revision > 0 (i.e. ACPI >= 2.0) */
        uint32_t length;
        uint64_t xsdt_addr;
        uint8_t xchecksum;
        uint8_t reserved[3];
} __attribute__((packed));

struct rsdt {
        struct sdt_header hdr;

        /* 32 bit addresses follow */
} __attribute__((packed));

/* extended rsdt which can hold addresses up to 64 bit
 * this needs to be used instead of the rsdt if possible */
struct xsdt {
        struct sdt_header hdr;

        /* 64 bit addresses follow */
} __attribute__((packed));

/* ---------- MADT ---------- */

struct madt_entry_header {
        uint8_t entry_type;
        uint8_t record_length;
} __attribute__((packed));

/* processor LAPIC */
struct madt_entry_lapic {
        struct madt_entry_header hdr;

        uint8_t acpi_processor_id;
        uint8_t apic_id;
        uint32_t flags; /* bit 0 = processor enabled, bit 1 = online capable */
} __attribute__((packed));

/* I/O APIC */
struct madt_entry_io_apic {
        struct madt_entry_header hdr;

        uint8_t io_apic_id;
        uint8_t reserved;
        uint32_t io_apic_addr;
        uint32_t gsib; /* global system interrupt base */
} __attribute__((packed));

/* interrupt source override */
struct madt_entry_iso {
        struct madt_entry_header hdr;

        uint8_t bus_source;
        uint8_t irq_source;
        uint32_t gsi;
        uint16_t flags;
} __attribute__((packed));

/* non-maskable interrupts */
struct madt_entry_nmi {
        struct madt_entry_header hdr;

        uint8_t acpi_processor_id;
        uint16_t flags;
        uint8_t lint;   /* LINT#(0 or 1) */
} __attribute__((packed));

/* local APIC addres override */
struct madt_entry_laao {
        struct madt_entry_header hdr;

        uint16_t reserved;
        uint64_t lapic_addr;
} __attribute__((packed));

struct madt {
        struct sdt_header hdr;

        uint32_t local_apic;
        uint32_t flags;

        /* variable length records follow */
} __attribute__((packed));

#endif
