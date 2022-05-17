#include "vfs.h"

#include "../libk/strutil.h"
#include "../libk/printf.h"

#include "fs.h"

fd_t vfs_file_open(const char *path)
{
        if (strlen(path) > MAX_PATH_LENGTH) {
                printf(KMSG_LOGLEVEL_WARN, "path too long, can't open file\n");
                return -1;
        }

        char p[MAX_PATH_LENGTH + 1];
        strcpy(p, path);

        size_t end = strfind(path, ':');
        if (end == NPOS) {
                printf(KMSG_LOGLEVEL_WARN, "invalid path\n");
                return -1;
        }

        p[end] = '\0';
        const char *drive = p;
        const char *path = p + end + 1;

        for (size_t mi = 0; mi < MAX_MOUNTLIST_SIZE; mi++) {
                const struct fs_mount *mount = &mountlist[mi];
                if (strcmp(mount->name, drive) == 0) {
                        printf(KMSG_LOGLEVEL_INFO, "processing path %s", path);
                }
        }
}
