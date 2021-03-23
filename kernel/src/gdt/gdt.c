#include "gdt.h"

#include <printk.h>

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
        printk(KMSG_LOGLEVEL_INFO, "Reached target gdt.\n");

        struct gdt_descriptor gdt;
        gdt.size = sizeof(struct gdt) - 1;
        gdt.offset = (uintptr_t)&default_gdt;

        printk(KMSG_LOGLEVEL_INFO, "gdt @ %x with size=%d, offset=%x\n",
               &gdt, gdt.size, gdt.offset);

        asm volatile("lgdt %0;"
                     "movw $0x10, %%ax;"
                     "movw %%ax, %%ds;"
                     "movw %%ax, %%es;"
                     "movw %%ax, %%fs;"
                     "movw %%ax, %%gs;"
                     "movw %%ax, %%ss;"
                     :
                     : "m" (gdt));

        printk(KMSG_LOGLEVEL_SUCC, "Finished target gdt.\n");
}
