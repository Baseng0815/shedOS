#ifndef _PARTITION_H
#define _PARTITION_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t part_id;

struct drive;

struct gpt_hdr {
        char signature[8];
        uint32_t revision;
        uint32_t hdr_size;
        uint32_t hdr_crc32;
        uint32_t reserved0;
        uint64_t current_lba;
        uint64_t backup_lba;
        uint64_t first_usable_lba;
        uint64_t last_usable_lba;
        uint64_t disk_guid[2];
        uint64_t pentry_start_lba;
        uint32_t pentry_count;
        uint32_t pentry_size;
        uint32_t pentry_crc32;
        uint8_t reserved1[420];
} __attribute__((packed));

struct gpt_entry {
        uint64_t type_guid[2];
        uint64_t part_guid[2];
        uint64_t first_lba;
        uint64_t last_lba;
        uint64_t attrs;
        uint16_t name[36];
} __attribute__((packed));

struct gpt {
        struct gpt_hdr hdr;
        part_id id; /* TODO is this even necessary? */
        size_t entries_count;
        struct gpt_entry *entries;
        const struct drive *drive;
};

void partition_load_drive(struct drive *drive);

#endif
