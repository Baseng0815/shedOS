#include "gdt.h"

#include "../libk/printf.h"

#define GDT_ENTRY(base, limit, access, flags) \
{ \
        (limit) & 0xffff, \
        (base) & 0xffff, \
        (base) >> 16 & 0xff, \
        (access) | PRESENT, \
        (limit) >> 16 | (flags) << 4, \
        (base) >> 24 \
}

enum desc_access {
        ACCESSED    = 1 << 0,   /* CPU sets to 1 when segment is accessed */
        RW          = 1 << 1,   /* writing to code is never allowed */
        DC          = 1 << 2,   /* direction bit */
        EXE         = 1 << 3,   /* executable */
        CODE_DATA   = 1 << 4,   /* type (set to 1 for code and data) */
        RING_1      = 1 << 5,   /* ring 1 DPL */
        RING_2      = 1 << 6,   /* ring 2 DPL */
        RING_3      = 1 << 5 | 1 << 6,   /* ring 3 DPL */
        PRESENT     = 1 << 7    /* must be 1 for all valid selectors */
};

enum desc_flags {
        LONG_GRAN   = 1 << 1,   /* x86-64 segment */
        SIZE        = 1 << 2,   /* 0/1 <=> 16/32 bit pmode */
        PAGE_GRAN   = 1 << 3    /* 0/1 <=> byte/page granularity */
};

extern void gdt_load(struct gdt_descriptor*);
extern void tss_load();

static struct tss tss;
static __attribute__((aligned(0x1000))) struct gdt gdt = {
        /* null @ 0x00 */
        GDT_ENTRY(0, 0, 0, 0),
        /* kernel code @ 0x08 */
        GDT_ENTRY(0, 0xfffff, RW | CODE_DATA | EXE, LONG_GRAN),
        /* kernel data @ 0x10 */
        GDT_ENTRY(0, 0xfffff, RW | CODE_DATA, 0),
        /* user code @ 0x18 */
        GDT_ENTRY(0, 0xfffff, RW | CODE_DATA | EXE | RING_3, LONG_GRAN),
        /* user data @ 0x20 */
        GDT_ENTRY(0, 0xfffff, RW | CODE_DATA | RING_3, 0)
        /* TSS lower @ 0x28 */
        /* initialized at runtime */
};

void gdt_initialize(void *kernel_stack)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target gdt.\n");

        gdt.tss.lower_entry =
                (struct gdt_entry)GDT_ENTRY((uintptr_t)&tss & 0xffffffff,
                                        sizeof(tss), EXE | ACCESSED, SIZE);
        gdt.tss.base3 = (uintptr_t)&tss >> 32;
        gdt.tss.reserved0 = 0;

        struct gdt_descriptor gdt_desc;
        gdt_desc.size = sizeof(gdt) - 1;
        gdt_desc.offset = (uintptr_t)&gdt;

        printf(KMSG_LOGLEVEL_INFO,
               "gdt descriptor at %a with size=%d, offset=%x\n",
               &gdt_desc, gdt_desc.size, gdt_desc.offset);

        gdt_load(&gdt_desc);
        printf(KMSG_LOGLEVEL_INFO, "GDT loaded.\n");

        memset(&tss, 0, sizeof(tss));
        tss.rsp0        = (uintptr_t)kernel_stack;
        tss.ist1        = (uintptr_t)kernel_stack;
        tss.iopb_offset = sizeof(tss);
        tss_load();
        printf(KMSG_LOGLEVEL_INFO,
               "TSS loaded with rsp0=%a, ist1=%a, iopb_offset=%x\n",
               tss.rsp0, tss.ist1, tss.iopb_offset);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target gdt.\n");
}
