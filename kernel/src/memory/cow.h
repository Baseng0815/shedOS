#ifndef _COW_H
#define _COW_H

/* copy-on-write implementation
 * instead of copying a whole address space when forking, we copy only on
 * page access (indicated by page fault exception) */

void cow_copy_on_fault(void *vaddr);

#endif
