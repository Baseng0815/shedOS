#ifndef _DRIVE_H
#define _DRIVE_H

#include <stdint.h>
#include <stddef.h>

#define DRIVE_TYPE_NVME 0

struct drive {
        uint8_t id;
        size_t block_size;

        void (*read_blocks)(uint8_t *buf, size_t block_count,
                            size_t block_offset, const struct drive *drive);
};

void drive_new(struct drive **drive);
void drive_finish_load(const struct drive *drive);
void drive_read(uint8_t *buf, size_t len, size_t offset,
                const struct drive *drive);

extern struct drive drives[256];
extern size_t drives_count;

#endif
