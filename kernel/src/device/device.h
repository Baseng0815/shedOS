#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdint.h>

#include "src/libk/util.h"
#include "../userspace/kernel_function.h"

/* A device is the highest level of abstraction for components the kernel can
 * interact with. Screens, drives, controllers etc. are all modeled as a device
 * with a class and class-specific data that is expected to be common to
 * all class devices. */

/* Design note: data is owned by the device struct and NOT by the subsystems.
 * Sharing data between devices is discouraged, even for exactly the same
 * functions offered by the same device class. */

typedef uint8_t dev_id;

enum device_class {
        DCLASS_KERNEL, /* the kernel gets treated as a device to expose functions */
        DCLASS_DRIVE, /* an external drive with block level access */
        DCLASS_COUNT
};

struct device {
        enum device_class class;
        struct kernel_function *functions[255]; /* functions exposed to userspace */
        dev_id id; /* TODO is this even necessary? */
        void *data;
};

void device_register(TAKE struct device *device);

/* complete device setup after subsystem has finalized its loading procedure */
void device_finalize(TAKE struct device *device);

#endif
