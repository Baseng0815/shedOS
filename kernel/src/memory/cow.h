#ifndef _COW_H
#define _COW_H

/* copy-on-write implementation
 * we only need to copy a page from the parent process when writing to it,
 * not when reading (write indicated by page fault) */

void cow_copy_on_write(void *vaddr);

#endif
