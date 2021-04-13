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
        uint32_t first_sdt;
} __attribute__((packed));

/* extended rsdt which can hold addresses up to 64 bit
 * this needs to be used instead of the rsdt if possible */
struct xsdt {
        struct sdt_header hdr;
        uint64_t first_sdt;
} __attribute__((packed));

struct madt_entry {
        uint8_t entry_type;
        uint8_t record_length;
} __attribute__((packed));

struct madt {
        struct sdt_header hdr;
        uint32_t local_apic;
        uint32_t flags;
        struct madt_entry *entries;
} __attribute__((packed));

#endif
