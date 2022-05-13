#ifndef _VFS_H
#define _VFS_H

typedef uint8_t fd_t;

fd_t vfs_file_open(const char *path);

#endif
