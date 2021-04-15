#include "gdt.h"

#include "../libk/printf.h"

extern void gdt_load(struct gdt_descriptor*);

static __attribute__((aligned(0x1000))) struct gdt default_gdt = {
        {0, 0, 0, 0x00, 0x00, 0},   // kernel null
        {0, 0, 0, 0x9a, 0xa0, 0},   // kernel code segment
        {0, 0, 0, 0x92, 0xa0, 0},   // kernel data segment
        {0, 0, 0, 0x00, 0x00, 0},   // user null
        {0, 0, 0, 0x9a, 0xa0, 0},   // user code segment
        {0, 0, 0, 0x92, 0xa0, 0}    // user data segment
};

void gdt_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target gdt.\n");

        struct gdt_descriptor gdt;
        gdt.size = sizeof(struct gdt) - 1;
        gdt.offset = (uintptr_t)&default_gdt;

        printf(KMSG_LOGLEVEL_INFO, "gdt @ %x with size=%d, offset=%x\n",
               &gdt, gdt.size, gdt.offset);

        gdt_load(&gdt);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target gdt.\n");
}
