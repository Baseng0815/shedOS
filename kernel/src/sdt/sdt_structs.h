#ifndef _SDT_STRUCTS_H
#define _SDT_STRUCTS_H

#include <stdint.h>

struct acpi_addr {
        uint8_t addr_space_id; /* 0 = system memory, 1 = system I/O */
        uint8_t register_bit_width;
        uint8_t register_bit_offset;
        uint8_t reserved;
        uint64_t addr;
} __attribute__((packed));

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

struct rsdp_rev1 {
        /* those fields are only valid if revision > 0 (i.e. ACPI >= 2.0) */
        uint32_t length;
        uint64_t xsdt_addr;
        uint8_t xchecksum;
        uint8_t reserved[3];
};

struct rsdp {
        char signature[8]; /* "RSD PTR " */
        uint8_t checksum;
        char oem_id[6];
        uint8_t revision;
        uint32_t rsdt_addr;

        struct rsdp_rev1 rev1;
} __attribute__((packed));

struct rsdt {
        struct sdt_header hdr;

        /* 32 bit addresses follow */
} __attribute__((packed));

/* extended rsdt which can hold addresses up to 64 bit
   this needs to be used instead of the rsdt if possible */
struct xsdt {
        struct sdt_header hdr;

        /* 64 bit addresses follow */
} __attribute__((packed));

/* ---------- MADT ---------- */

enum madt_entry_type {
        ENTRY_LAPIC     = 0x0,
        ENTRY_IOAPIC    = 0x1,
        ENTRY_ISO       = 0x2,
        ENTRY_NMI       = 0x4,
        ENTRY_LAAO      = 0x5
};

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
struct madt_entry_ioapic {
        struct madt_entry_header hdr;

        uint8_t ioapic_id;
        uint8_t reserved;
        uint32_t ioapic_addr;
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

/* ---------- HPET ---------- */

/* high precision event timer */
struct hpet {
        struct sdt_header hdr;

        uint8_t hardware_rev_id;
        uint8_t comparator_count    : 5;
        uint8_t count_size_cap      : 1;
        uint8_t reserved            : 1;
        uint8_t legacy_replacement  : 1;
        uint16_t pci_vendor_id;
        struct acpi_addr addr;
        uint8_t hpet_number;
        uint16_t min_clock_ticks; /* minimum clock_tick in period mode */
} __attribute__((packed));

/* ---------- MCFG ---------- */

/* configuration space base address allocation structure */
struct mcfg_csbaas {
        uint64_t base_address;
        uint16_t seg_group_number;
        uint8_t start_bus_number;
        uint8_t end_bus_number;
        uint32_t reserved;
} __attribute__((packed));

/* memory mapped configuration space access used for PCIe */
struct mcfg {
        struct sdt_header hdr;

        uint64_t reserved0;
        struct mcfg_csbaas csbaas[];
} __attribute__((packed));

#endif
