#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

#define MAX_PATH_LENGTH (126)

typedef int8_t fd_t;

fd_t vfs_file_open(const char *_path);

#endif
