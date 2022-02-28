#include "gdt.h"

#include "../libk/memutil.h"
#include "../libk/printf.h"

/* descriptor access */
#define DA_ACCESSED     (1UL << 0)  /* CPU sets to 1 when segment is accessed */
#define DA_RW           (1UL << 1)  /* writing to code is never allowed */
#define DA_DC           (1UL << 2)  /* direction bit */
#define DA_EXE          (1UL << 3)  /* executable */
#define DA_CODE_DATA    (1UL << 4)  /* type (set to 1 for code and data) */
#define DA_RING_1       (1UL << 5)  /* ring 1 DPL */
#define DA_RING_2       (1UL << 6)  /* ring 2 DPL */
#define DA_RING_3       (DA_RING_1 | DA_RING_2)  /* ring 3 DPL */
#define DA_PRESENT      (1UL << 7)  /* must be 1 for all valid selectors */

/* descriptor flags */
#define DF_LONG_GRAN    (1UL << 1)  /* x86-64 code segment */
#define DF_SIZE         (1UL << 2)  /* 0/1 <=> 16/32 bit pmode */
#define DF_PAGE_GRAN    (1UL << 3)  /* 0/1 <=> byte/page granularity */

#define GDT_ENTRY(base, limit, access, flags) \
{ \
        (limit) & 0xffff, \
        (base) & 0xffff, \
        (base) >> 16 & 0xff, \
        (access) | DA_PRESENT, \
        (limit) >> 16 | (flags) << 4, \
        (base) >> 24 \
}

extern void gdt_load(struct gdt_descriptor*);
extern void tss_load();

static struct tss tss;
static __attribute__((aligned(0x1000))) struct gdt gdt = {
        /* null @ 0x00 */
        GDT_ENTRY(0, 0, 0, 0),
        /* kernel code @ 0x08 */
        GDT_ENTRY(0, 0xfffff, DA_RW | DA_CODE_DATA | DA_EXE, DF_LONG_GRAN),
        /* kernel data @ 0x10 */
        GDT_ENTRY(0, 0xfffff, DA_RW | DA_CODE_DATA, 0),
        /* user code @ 0x18 */
        GDT_ENTRY(0, 0xfffff, DA_RW | DA_CODE_DATA | DA_EXE | DA_RING_3,
                  DF_LONG_GRAN),
        /* user data @ 0x20 */
        GDT_ENTRY(0, 0xfffff, DA_RW | DA_CODE_DATA | DA_RING_3, 0)
        /* TSS lower @ 0x28 */
        /* initialized at runtime */
};

void gdt_initialize(void *kernel_stack)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target gdt.\n");

        gdt.tss.lower_entry =
                (struct gdt_entry)GDT_ENTRY((uintptr_t)&tss & 0xffffffff,
                                        sizeof(tss), DA_EXE | DA_ACCESSED, DF_SIZE);
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
        tss.ist1        = 0;
        tss.iopb_offset = sizeof(tss);
        tss_load();
        printf(KMSG_LOGLEVEL_INFO,
               "TSS loaded with rsp0=%a, ist1=%a, iopb_offset=%x\n",
               tss.rsp0, tss.ist1, tss.iopb_offset);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target gdt.\n");
}
