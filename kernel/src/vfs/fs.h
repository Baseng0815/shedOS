#ifndef _FS_H
#define _FS_H

#define MAX_MOUNTLIST_SIZE

struct fs {
        const char id;
};

struct fs_mount {
        struct fs *fs;
        const char *name;
};

extern struct fs_mount mountlist[MAX_MOUNTLIST_SIZE];

#endif
