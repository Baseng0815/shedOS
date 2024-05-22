#include "drive.h"
#include "../../libk/alloc.h"
#include "../../libk/memutil.h"

void drive_read(uint8_t *buf, size_t len, size_t offset, struct drive *drive)
{
        size_t bs = drive->block_size;

        /* quick maths */
        size_t lba_start        = offset / bs;
        size_t lba_start_offset = offset & (bs - 1);
        size_t lba_end          = (offset + (len - 1)) / bs;

        size_t block_count  = lba_end - lba_start + 1;
        size_t page_count   = (block_count * bs + 0xfff) / 0x1000;

        uint8_t *buf_blocks = palloc(page_count);
        drive->read_blocks(buf_blocks, block_count, lba_start, drive);
        memcpy(buf, buf_blocks + lba_start_offset, len);

        pfree(buf_blocks, page_count);
}
