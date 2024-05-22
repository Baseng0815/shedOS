#include "partition.h"

#include "../device/drive/drive.h"
#include "../libk/printf.h"
#include "../libk/alloc.h"
#include "../libk/strutil.h"
#include "../debug.h"

/* TODO abstract into partition to also allow things like MBR */
static struct gpt *partitions[255] = { 0 };

void partition_load_drive(struct drive *drive)
{
        struct gpt *gpt = zmalloc(sizeof(struct gpt));
        gpt->drive = drive;

        printf(KMSG_LOGLEVEL_INFO,
               "Loading partition table of drive...\n");

        drive_read((uint8_t*)&gpt->hdr, sizeof(struct gpt_hdr),
                   drive->block_size, drive);
        assert(strncmp(gpt->hdr.signature, "EFI PART", 8) == 0,
               "Invalid GPT signature");

        printf(KMSG_LOGLEVEL_INFO,
               "Current lba: %d, backup lba: %d, "
               "first usable lba: %d, last usable lba: %d, "
               "pentry count: %d, pentry size: %x, pentry start lba: %d\n",
               gpt->hdr.current_lba, gpt->hdr.backup_lba,
               gpt->hdr.first_usable_lba, gpt->hdr.last_usable_lba,
               gpt->hdr.pentry_count, gpt->hdr.pentry_size,
               gpt->hdr.pentry_start_lba);

        gpt->entries = malloc(gpt->hdr.pentry_count * gpt->hdr.pentry_size, 0);

        // LBA 2..33 contain partition table entries
        uint8_t *lba2 = palloc((drive->block_size * 16 + 0xfff) / 0x1000);
        drive_read(lba2, drive->block_size * 16,
                   drive->block_size * gpt->hdr.pentry_start_lba, drive);

        gpt->entries_count = 0;
        for (size_t i = 0; i < gpt->hdr.pentry_count; i++) {
                uint64_t off = i * gpt->hdr.pentry_size;
                const struct gpt_entry *gpt_entry =
                        (const struct gpt_entry*)&lba2[off];

                if (gpt_entry->first_lba == 0)
                        continue;

                printf(KMSG_LOGLEVEL_INFO,
                       "Partition %d spanning (%d,%d) found\n",
                       gpt->entries_count, gpt_entry->first_lba,
                       gpt_entry->last_lba);

                gpt->entries[gpt->entries_count++] = *gpt_entry;
        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished loading partition table.\n");
        for (part_id id = 0; id < 255; id++) {
                if (partitions[id] == NULL) {
                        partitions[id] = gpt;
                        gpt->id = id;
                        break;
                }
        }

        pfree(lba2, (drive->block_size + 0xfff) / 0x1000);
}
