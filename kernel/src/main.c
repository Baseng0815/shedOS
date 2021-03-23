#include "framebuffer.h"
#include "font.h"
#include "terminal.h"
#include "cpuinfo.h"

#include "memory/pfa.h"
#include "memory/paging.h"
#include "memory/efi_memory.h"

#include <printk.h>
#include <string.h>

struct bootinfo {
        struct framebuffer      *framebuffer;
        struct psf1_font        *font;
        struct efi_memory_map    *memory_map;
};

void welcome_message();
void dump_bootinfo(struct bootinfo*);
void dump_cpu();
void dump_memory(struct efi_memory_map*);

void _start(struct bootinfo *bootinfo)
{
        /* initialize framebuffer, font and terminal */
        struct framebuffer *fb = bootinfo->framebuffer;

        framebuffer_initialize(fb);
        font_initialize(bootinfo->font);
        /* leave room for 8px border on each side */
        int term_width = (fb->width - 16) / 8;
        int term_height = (fb->height - 16) / 16;
        terminal_initialize(term_width, term_height);
        printk(KMSG_LOGLEVEL_SUCC,
               "Framebuffer, font and terminal initialized with "
               "dimensions of %dx%d.\n",
               term_width, term_height);

        struct efi_memory_map *mmap = bootinfo->memory_map;
        welcome_message();
        dump_bootinfo(bootinfo);
        dump_memory(bootinfo->memory_map);
        dump_cpu();

        /* initialize page frame allocator */
        pfa_initialize(mmap, fb);

        /* initialize identity paging */
        printk(KMSG_LOGLEVEL_INFO, "Reached target identity paging.\n");
        struct page_table *pml4 = paging_identity(fb);
        printk(KMSG_LOGLEVEL_INFO, "Kernel page map @ %x\n", pml4);
        printk(KMSG_LOGLEVEL_SUCC, "Finished target identity paging.\n");

        printk(KMSG_LOGLEVEL_WARN,
               "Kernel finished. You are now hanging in an infinite loop. "
               "Congratulations :)\n");

        while (true) {}
}

void welcome_message()
{
        const char *ascii =
                "\n     _              _  ___  ____\n"
                " ___| |__   ___  __| |/ _ \\/ ___|\n"
                "/ __| '_ \\ / _ \\/ _` | | | \\___ \\\n"
                "\\__ \\ | | |  __/ (_| | |_| |___) |\n"
                "|___/_| |_|\\___|\\__,_|\\___/|____/\n\n";

        printk(KMSG_LOGLEVEL_NONE, ascii);
}

void dump_bootinfo(struct bootinfo *bootinfo)
{
        struct framebuffer      *fb     = bootinfo->framebuffer;
        struct efi_memory_map   *mmap   = bootinfo->memory_map;

        printk(KMSG_LOGLEVEL_INFO,
               "bootinfo @ %x\n"
               "|-> framebuffer @ %x\n"
               "|---> paddr=%x, size=%dKiB, %dx%dpx, pitch=%d bytes\n"
               "|-> font @ %x\n"
               "|---> header @ %x\n"
               "|-----> mode=%d, charsize=%d\n"
               "|---> glyphs @ %x\n"
               "|-> mmap @ %x\n"
               "|---> paddr=%x, size=%d bytes, desc_size=%d bytes\n",
               bootinfo,
               fb,
               fb->addr, fb->size / 1024, fb->width, fb->height, fb->pitch,
               bootinfo->font,
               bootinfo->font->header,
               bootinfo->font->header->mode, bootinfo->font->header->charsize,
               bootinfo->font->glyphs,
               mmap,
               mmap->paddr, mmap->size, mmap->desc_size);
}

void dump_cpu()
{
        struct cpuinfo cpuinfo;
        bool cpuid_present = cpuinfo_query(&cpuinfo);
        if (cpuid_present) {
                printk(KMSG_LOGLEVEL_INFO, "CPU: %s (%s)\n",
                       cpuinfo.vendor_string, cpuinfo.brand_string);
                printk(KMSG_LOGLEVEL_NONE,
                       "|-> stepping=%d, model=%d, family=%d, "
                       "processor_type=%d\n",
                       cpuinfo.stepping, cpuinfo.model, cpuinfo.family,
                       cpuinfo.processor_type);
                printk(KMSG_LOGLEVEL_NONE, "|-> features: ");
                for (int i = 0; i < 64; i++) {
                        /* reserved bits */
                        if (i == 10 || i == 20 || i == 47)
                                continue;

                        if ((cpuinfo.featureset & (1L << i)) > 0) {
                                printk(KMSG_LOGLEVEL_NONE,
                                       "%s ", cpu_featureset[i]);
                        }
                }
                printk(KMSG_LOGLEVEL_NONE, "\n");
        }
}

void dump_memory(struct efi_memory_map *mmap)
{
        printk(KMSG_LOGLEVEL_INFO,
               "Dumping memory map entries of type EfiConventionalMemory...\n");

        size_t mm_count = mmap->size / mmap->desc_size;
        for (size_t i = 0; i < mm_count; i++) {
                struct efi_memory_descriptor *md =
                        (struct efi_memory_descriptor*)
                        (mmap->paddr + (i * mmap->desc_size));

                if (md->type != 7)
                        continue;

                printk(KMSG_LOGLEVEL_NONE,
                       "|-> type=%s(%d), size=%dKiB, paddr=%x, pc=%d\n",
                       efi_memory_strings[md->type], md->type,
                       md->page_count * 4096 / 1024, md->paddr, md->page_count);
        }
}
