#include "vfs.h"

#include "../libk/strutil.h"
#include "../libk/printf.h"

fd_t vfs_file_open(const char *_path)
{
        if (strlen(_path) > MAX_PATH_LENGTH) {
                printf(KMSG_LOGLEVEL_WARN, "path too long, can't open file\n");
                return -1;
        }

        char drive = _path[0];
        const char *path = _path + 2;

        for (size_t mi = 0; mi < 26; mi++) {
                printf(KMSG_LOGLEVEL_INFO, "processing path %s\n", path);
        }

        return 0;
}
