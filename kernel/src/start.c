#include <stdint.h>
#include <stddef.h>

#include "cpuinfo.h"

#include "libk/printf.h"
#include "libk/kmalloc.h"

#include "fb/framebuffer.h"
#include "terminal/terminal.h"

#include "memory/pmm.h"
#include "memory/paging.h"

#include "gdt/gdt.h"
#include "sdt/sdt.h"

#include "interrupts/timer.h"
/* #include "interrupts/apic.h" */
#include "interrupts/pic.h"
#include "interrupts/idt.h"

#include "libk/kmalloc.h"
#include "pci/pci.h"
#include "user.h"

static uint8_t stack_kernel[0x4000]; /* 16 KiB stack */
static uint8_t stack_user[0x4000]; /* 16 KiB stack */

static void welcome_message();
static void dump_stivale_info(struct stivale2_struct*);
static void dump_cpu();
static void dump_memory(struct stivale2_struct_tag_memmap*);

/* the kernel uses a linked list of structures to communicate
   with the bootloader and request certain features */
/* header tags are given to the bootloader from the kernel,
   structure tags are returned from the bootloader to the kernel */

static struct stivale2_tag unmap_null_header_tag = {
        .identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID,
        .next = NULL
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
        .tag = {
                .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
                /* zero indicates the end of the linked list */
                .next = &unmap_null_header_tag
        },
#ifdef FB_FHD
        .framebuffer_width  = 1920,
        .framebuffer_height = 1080,
#elif defined(FB_QHD)
        .framebuffer_width  = 2560,
        .framebuffer_height = 1440,
#endif
        .framebuffer_bpp    = 32
};

/* we put the stivale header into a certain section to
   allow the bootloader to find it */
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header header = {
        .entry_point = 0,
        .stack = (uintptr_t)stack_kernel + sizeof(stack_kernel),
        .flags = 2,
        .tags = (uintptr_t)&framebuffer_hdr_tag
};

/* helper function to traverse the linked list and get the struct we want */
void *stivale2_get_tag(struct stivale2_struct*, uint64_t);

void _start(struct stivale2_struct *stivale2_struct)
{
        asm volatile("cli");

        /* framebuffer and terminal */
        struct stivale2_struct_tag_framebuffer *fb =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

        framebuffer_initialize(fb);
        int term_width = (fb->framebuffer_width - 16) / 8;
        int term_height = (fb->framebuffer_height - 16) / 16;
        terminal_initialize(term_width, term_height);

        welcome_message();
        dump_stivale_info(stivale2_struct);

        printf(KMSG_LOGLEVEL_OKAY,
               "Framebuffer initialized with dimension of %dx%d, "
               "base address of %a, "
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
        pmm_initialize(mmap);
        paging_initialize(mmap, fb);
        gdt_initialize((uintptr_t)stack_kernel + sizeof(stack_kernel));

        /* system descriptor tables */
        struct stivale2_struct_tag_rsdp *rsdp =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_RSDP_ID);

        sdt_initialize(rsdp);

        /* interrupts */
        idt_initialize();
        apic_initialize(madt);
        timer_initialize();
        asm volatile("sti");
        kmalloc_initialize();

        /* pci_init(); */

        /* this is temporary */
        _user_jump((uintptr_t)stack_user + sizeof(stack_user));

        printf(KMSG_LOGLEVEL_OKAY,
               "Kernel initialization completed.\n");

        for (;;) {
                asm volatile("hlt");
        }
}

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
        for (struct stivale2_tag *current_tag = (void*)stivale2_struct->tags;
             current_tag != NULL;
             current_tag = (void*)current_tag->next) {
                if (current_tag->identifier == id)
                        return current_tag;
        }

        return NULL;
}

void welcome_message()
{
        const char *banner =
                "\n     _              _  ___  ____\n"
                " ___| |__   ___  __| |/ _ \\/ ___|\n"
                "/ __| '_ \\ / _ \\/ _` | | | \\___ \\\n"
                "\\__ \\ | | |  __/ (_| | |_| |___) |\n"
                "|___/_| |_|\\___|\\__,_|\\___/|____/\n\n";

        printf(KMSG_LOGLEVEL_NONE, banner);
}

void dump_stivale_info(struct stivale2_struct *stivale2_struct)
{
        struct stivale2_struct_tag_epoch *epoch =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_EPOCH_ID);
        if (epoch)
                printf(KMSG_LOGLEVEL_INFO, "UNIX epoch at boot: %d\n",
                       epoch->epoch);

        struct stivale2_struct_tag_firmware *firmware =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_FIRMWARE_ID);
        if (firmware) {
                printf(KMSG_LOGLEVEL_INFO, "Firmware: %s\n",
                       firmware->flags & 0x1 ? "UEFI" : "BIOS");

                if (firmware->flags & 0x1) {
                        uint64_t tag = STIVALE2_STRUCT_TAG_EFI_SYSTEM_TABLE_ID;
                        struct stivale2_struct_tag_efi_system_table *efi_table =
                                stivale2_get_tag(stivale2_struct, tag);
                        if (efi_table) {
                                printf(KMSG_LOGLEVEL_INFO, "EFI system table at %a\n",
                                       efi_table->system_table);
                        }
                }
        }

        struct stivale2_struct_tag_cmdline *cmdline =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_CMDLINE_ID);
        if (cmdline) {
                printf(KMSG_LOGLEVEL_INFO, "Kernel cmdline: %s\n",
                       (const char*)cmdline->cmdline);
        }

        struct stivale2_struct_tag_modules *modules =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_MODULES_ID);
        if (modules) {
                printf(KMSG_LOGLEVEL_INFO, "%d modules loaded\n",
                       modules->module_count);
                for (size_t i = 0; i < modules->module_count; i++) {
                        printf(KMSG_LOGLEVEL_NONE, "|-> begin=%a, end=%a, desc=%s\n",
                               modules->modules[i].begin, modules->modules[i].end,
                               modules->modules[i].string);
                }
        }

        struct stivale2_struct_tag_kernel_file *kernel_file =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
        if (kernel_file) {
                printf(KMSG_LOGLEVEL_INFO, "Kernel file at %a\n",
                       kernel_file->kernel_file);
        }

        struct stivale2_struct_tag_kernel_slide *kernel_slide =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_KERNEL_SLIDE_ID);
        if (kernel_slide) {
                printf(KMSG_LOGLEVEL_INFO, "Kernel slide of %x\n",
                       kernel_slide->kernel_slide);
        }

        struct stivale2_struct_tag_mmio32_uart *mmio_uart =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_MMIO32_UART);
        if (mmio_uart) {
                printf(KMSG_LOGLEVEL_INFO, "MMIO UART at %a\n",
                       mmio_uart->addr);
        }

        struct stivale2_struct_vmap *vmap =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_VMAP);
        if (vmap) {
                printf(KMSG_LOGLEVEL_INFO, "VMAP_HIGH=%a\n", vmap->addr);
        }
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
        printf(KMSG_LOGLEVEL_INFO, "Memory map:\n");
        for (size_t i = 0; i < mmap->entries; i++) {
                struct stivale2_mmap_entry *entry = &mmap->memmap[i];

                const char *type;
                /* TODO replace with array */
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
                       "|-> %a - %a, type=%s\n",
                       entry->base, entry->base + entry->length, type);
        }
}
