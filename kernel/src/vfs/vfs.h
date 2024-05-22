#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

#include "../device/drive/drive.h"

#define MAX_PATH_LENGTH 126

typedef int8_t fd_t;

void vfs_register_drive(struct drive *drive);
fd_t vfs_file_open(const char *_path);

#endif
