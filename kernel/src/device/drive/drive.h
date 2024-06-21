#ifndef _DRIVE_H
#define _DRIVE_H

#include "../device.h"

#include <stdint.h>
#include <stddef.h>

/* A drive with block-granular read/write-access */

struct drive {
        size_t block_size;
        /* function exposed to kernel */
        void (*read_blocks)(uint8_t *buf, size_t block_count,
                         size_t block_offset, struct drive *drive);
        void (*destroy)(void); /* TODO is this needed? (right now: no) */

        void *data; /* subsystem data (e.g. NVME) */
};

void drive_read(uint8_t *buf, size_t count, size_t offset, struct drive *drive);

#endif
