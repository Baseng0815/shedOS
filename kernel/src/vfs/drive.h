#ifndef _DRIVE_H
#define _DRIVE_H

#include <stdint.h>
#include <stddef.h>

#define DRIVE_TYPE_NVME 0

struct drive {
        uint8_t id;
        uint8_t type;
        size_t block_size;

        void (*read)(uint8_t *buf, size_t len, size_t offset);
};

void drive_new(struct drive **drive);

extern struct drive drives[26];

#endif
