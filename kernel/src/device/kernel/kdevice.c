#include "kdevice.h"

#include "../../libk/printf.h"
#include "../../libk/strutil.h"
#include "../../libk/alloc.h"

#include "../device.h"
#include "../../userspace/kernel_function.h"

static void kfunction_hello(void *out, ...)
{
        strcpy(out, "GENERAL KENOBI");
}

void kdevice_register_functions(void)
{
        printf(KMSG_LOGLEVEL_INFO,
               "Reached target kernel function registration.\n");

        struct kernel_function *hello = zmalloc(sizeof(struct kernel_function));

        hello->name        = "HELLO THERE";
        hello->description = "Write 'GENERAL KENOBI' to the output"
                             "location. Useful for debugging.";
        hello->execute     = &kfunction_hello;

        struct device *kernel_device = zmalloc(sizeof(struct device), 0);
        kernel_device->class = DCLASS_KERNEL;
        kernel_device->functions[0] = hello;

        device_register(kernel_device);

        printf(KMSG_LOGLEVEL_OKAY,
               "Finished target kernel function registration.\n");
}
