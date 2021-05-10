#include <stdint.h>
#include <stddef.h>

#include "cpuinfo.h"

#include "libk/printf.h"

#include "fb/framebuffer.h"
#include "terminal/terminal.h"

#include "memory/pfa.h"
#include "memory/paging.h"

#include "gdt/gdt.h"
#include "sdt/sdt.h"

#include "interrupts/timer.h"
/* #include "interrupts/apic.h" */
#include "interrupts/pic.h"
#include "interrupts/idt.h"

static uint8_t stack[0x4000]; /* 16 KiB stack */

static void welcome_message();
static void dump_bootinfo(struct bootinfo*);
static void dump_cpu();
static void dump_memory(struct stivale2_struct_tag_memmap*);

/* the kernel uses a linked list of structures to communicate
   with the bootloader and request certain features */
/* header tags are given to the bootloader from the kernel,
   structure tags are returned by the bootloader to the kernel */

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
        .tag = {
                .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
                /* zero indicates the end of the linked list */
                .next = 0
        },
        .framebuffer_width  = 1920,
        .framebuffer_height = 1080,
        .framebuffer_bpp    = 32
};

/* we put the stivale header into a certain section to
   allow the bootloader to find it */
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header header = {
        .entry_point = 0,
        .stack = (uintptr_t)stack + sizeof(stack),
        .flags = 0,
        .tags = (uintptr_t)&framebuffer_hdr_tag
};

/* helper function to traverse the linked list and get the struct we want */
void *stivale2_get_tag(struct stivale2_struct*, uint64_t);

void _start(struct stivale2_struct *stivale2_struct)
{
        /* framebuffer and terminal */
        struct stivale2_struct_tag_framebuffer *fb =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

        framebuffer_initialize(fb);
        int term_width = (fb->framebuffer_width - 16) / 8;
        int term_height = (fb->framebuffer_height - 16) / 16;
        terminal_initialize(term_width, term_height);

        welcome_message();

        printf(KMSG_LOGLEVEL_OKAY,
               "Framebuffer initialized with dimension of %dx%d, "
               "base address of %x, "
               "red mask/size %x/%x, "
               "green mask/size %x/%x, "
               "blue mask/size %x/%x "
               "and %dbpp.\n",
               fb->framebuffer_width, fb->framebuffer_height,
               fb->framebuffer_addr,
               fb->red_mask_shift, fb->red_mask_size,
               fb->green_mask_shift, fb->green_mask_size,
               fb->blue_mask_shift, fb->blue_mask_size,
               fb->framebuffer_bpp);

        /* processor information */
        cpuinfo_initialize();
        dump_cpu();

        /* memory */
        struct stivale2_struct_tag_memmap *mmap =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_MEMMAP_ID);
        dump_memory(mmap);
        pfa_initialize(mmap);
        gdt_initialize();
        paging_initialize(mmap);

        /* system descriptor tables */
        struct stivale2_struct_tag_rsdp *rsdp =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_RSDP_ID);

        sdt_initialize(rsdp);

        /* interrupts */
        idt_initialize();
        /* pic_initialize(); */
        apic_initialize(madt);
        timer_initialize();

        printf(KMSG_LOGLEVEL_OKAY,
               "Kernel initialization completed.\n");

        for (;;) {
                printf(KMSG_LOGLEVEL_WARN, "we are finished XDDDD\n");
        }
}

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
        for (struct stivale2_tag *current_tag = (void*)stivale2_struct->tags;
             current_tag;
             current_tag = (void*)current_tag->next) {
                if (current_tag->identifier == id)
                        return current_tag;
        }

        return NULL;
}

void welcome_message()
{
        const char *ascii =
                "\n     _              _  ___  ____\n"
                " ___| |__   ___  __| |/ _ \\/ ___|\n"
                "/ __| '_ \\ / _ \\/ _` | | | \\___ \\\n"
                "\\__ \\ | | |  __/ (_| | |_| |___) |\n"
                "|___/_| |_|\\___|\\__,_|\\___/|____/\n\n";

        printf(KMSG_LOGLEVEL_NONE, ascii);
}

void dump_cpu()
{
        printf(KMSG_LOGLEVEL_NONE,
               "CPU: %s (%s)\n"
               "|-> stepping=%d, model=%d, family=%d, " "processor_type=%d\n"
               "|-> base freq=%d, max freq=%d, bus freq=%d\n"
               "|-> tsc=%d/%d, core crystal freq=%d\n",
               cpuinfo.vendor_string, cpuinfo.brand_string,
               cpuinfo.stepping, cpuinfo.model, cpuinfo.family,
               cpuinfo.processor_type,
               cpuinfo.base_frequency, cpuinfo.max_frequency,
               cpuinfo.bus_frequency,
               cpuinfo.tsc_ratio_numer, cpuinfo.tsc_ratio_denom,
               cpuinfo.core_crystal_freq);

        printf(KMSG_LOGLEVEL_NONE, "|-> features: ");

        for (int i = 0; i < 64; i++) {
                /* reserved bits */
                if (i == 10 || i == 20 || i == 47)
                        continue;

                if ((cpuinfo.featureset >> i) & 0x1) {
                        printf(KMSG_LOGLEVEL_NONE,
                               "%s ", cpu_featureset[i]);
                }
        }
        printf(KMSG_LOGLEVEL_NONE, "\n");
}

void dump_memory(struct stivale2_struct_tag_memmap *mmap)
{
        printf(KMSG_LOGLEVEL_INFO, "Dumping stivale memory map...\n");
        for (size_t i = 0; i < mmap->entries; i++) {
                struct stivale2_mmap_entry *entry = &mmap->memmap[i];

                const char *type;
                switch (entry->type) {
                        case 0x1:
                                type = "USABLE";
                                break;
                        case 0x2:
                                type = "RESERVED";
                                break;
                        case 0x3:
                                type = "ACPI_RECLAIMABLE";
                                break;
                        case 0x4:
                                type = "ACPI_NVS";
                                break;
                        case 0x5:
                                type = "BAD_MEMORY";
                                break;
                        case 0x1000:
                                type = "BOOTLOADER_RECLAIMABLE";
                                break;
                        case 0x1001:
                                type = "KERNEL_AND_MODULES";
                                break;
                        default:
                                type = "UNKNOWN";
                }

                printf(KMSG_LOGLEVEL_NONE,
                       "|-> %d: base=%x, length=%x, type=%s\n",
                       i, entry->base, entry->length, type);
        }
}
