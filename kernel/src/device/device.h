#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdint.h>

/* A device is the highest level of abstraction for components the kernel can
 * interact with. Screens, drives, controllers etc. are all modeled as a device
 * with a class and class-specific data that is expected to be common to
 * all class devices. */

typedef uint8_t dev_id;

enum device_class {
        DCLASS_DRIVE, /* an external drive with block level access */
        DCLASS_COUNT
};

struct device {
        enum device_class class;
        dev_id id; /* TODO is this even necessary? */
        void *data;
};

void device_register(struct device *device);

/* complete device setup after subsystem has finalized its loading procedure */
void device_finalize(struct device *device);

#endif
