#ifndef _FUNCTION_LIST
#define _FUNCTION_LIST

#include "src/libk/util.h"
#include <stdint.h>

/* All interaction between userspace and the kernel happens through functions
 * exposed by the kernel and various devices which are called using syscalls. */

typedef uint8_t function_code;

struct kernel_function {
        function_code code; /* TODO is this even necessary? */
        const char *name; /* Used for searching since the set of possible
                             functions is assumed to be larger than the set of
                             functions for a single device */
        const char *description;
        void(*execute)(void *out, ...);
};

/* For functions directly provided by the kernel. If a function is provided by
 * a device instead, add it to the device's function list */
void kernel_function_register(TAKE struct kernel_function *function);

#endif
