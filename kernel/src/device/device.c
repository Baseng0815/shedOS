#include "device.h"
#include "../libk/printf.h"
#include "src/debug.h"
#include "src/vfs/vfs.h"

struct device *devices[255] = { 0 };

void device_register(struct device *device)
{
        for (dev_id id = 0; id < 255; id++) {
                if (devices[id] == NULL) {
                        devices[id] = device;
                        device->id = id;
                        return;
                }
        }

        kernel_panic("Device capacity exceeded", __FILE__, __LINE__);
}

void device_finalize(struct device *device)
{
        switch (device->class) {
                case DCLASS_DRIVE:
                        vfs_register_drive(device->data);

                default:
                        break;
        }
}
