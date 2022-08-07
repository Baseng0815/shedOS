#include "drive.h"

struct drive drives[26] = { 0 };
static size_t drives_count = 0;

void drive_new(struct drive **drive)
{
        drives[drives_count].id = drives_count;
        *drive = &drives[drives_count++];
}
