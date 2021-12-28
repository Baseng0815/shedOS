#ifndef _COW_H
#define _COW_H

/* copy-on-write implementation
 * we only need to copy a page from the parent process when writing to it,
 * not when reading (write indicated by page fault) */

#include <stdbool.h>

/* return false on failure (i.e. page was read-only or kernel to begin with) */
bool cow_copy_on_write(void *vaddr);

#endif
