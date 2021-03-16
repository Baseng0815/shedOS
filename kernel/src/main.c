#include "framebuffer.h"
#include "font.h"
#include "terminal.h"
#include "memory/efi_memory.h"
#include "cpuinfo.h"

#include <printk.h>

struct bootinfo {
        struct framebuffer  *framebuffer;
        struct psf1_font    *font;
        void                *mmap;
        size_t              mm_size;
        size_t              md_size;
};

void _start(struct bootinfo *bootinfo)
{
        struct framebuffer *fb = bootinfo->framebuffer;

        framebuffer_initialize(fb);
        font_initialize(bootinfo->font);
        /* leave room for 8px border on each side */
        int term_width = (fb->width - 16) / 8;
        int term_height = (fb->height - 16) / 16;
        terminal_initialize(term_width, term_height);

        printk(KMSG_URGENCY_LOW,
               "Framebuffer, font and terminal initialized.\n"
               "bootinfo @ %x\n"
               "|-> framebuffer @ %x\n"
               "|---> addr @ %x, size=%dKiB, %dx%dpx, pitch=%d bytes\n"
               "|-> font @ %x\n"
               "|---> header @ %x\n"
               "|-----> mode=%d, charsize=%d\n"
               "|---> glyphs @ %x\n"
               "|-> mmap @ %x, mm_size=%d bytes, md_size=%d bytes\n",
               bootinfo,
               fb,
               fb->addr, fb->size / 1024, fb->width, fb->height, fb->pitch,
               bootinfo->font,
               bootinfo->font->header,
               bootinfo->font->header->mode, bootinfo->font->header->charsize,
               bootinfo->font->glyphs,
               bootinfo->mmap, bootinfo->mm_size, bootinfo->md_size);

        printk(KMSG_URGENCY_LOW,
               "Term dimensions: %dx%d\n",
               term_width, term_height);

        printk(KMSG_URGENCY_LOW,
               "Dumping memory map entries with 16 or more pages...\n");

        size_t mm_entries = bootinfo->mm_size / bootinfo->md_size;
        for (size_t i = 0; i < mm_entries; i++) {
                struct efi_memory_descriptor *md =
                        (struct efi_memory_descriptor*)
                        (bootinfo->mmap + (i * bootinfo->md_size));
                if (md->page_count < 16) { continue; }

                printk(KMSG_URGENCY_LOW,
                       "|-> type=%s(%d), size=%dKiB, paddr=%x, pc=%d\n",
                       efi_memory_strings[md->type], md->type,
                       md->page_count * 4096 / 1024, md->paddr, md->page_count);
        }

        struct cpuinfo cpuinfo;
        bool cpuid_present = cpuinfo_query(&cpuinfo);
        if (cpuid_present) {
                /* dump some CPU information */
                printk(KMSG_URGENCY_LOW, "CPU: %s (%s)\n",
                       cpuinfo.vendor_string, cpuinfo.brand_string);
                printk(KMSG_URGENCY_LOW,
                       "|-> stepping=%d, model=%d, family=%d, "
                       "processor_type=%d\n",
                       cpuinfo.stepping, cpuinfo.model, cpuinfo.family,
                       cpuinfo.processor_type);
                printk(KMSG_URGENCY_LOW, "|-> features: ");
                for (int i = 0; i < 64; i++) {
                        /* reserved bits */
                        if (i == 10 || i == 20 || i == 47) {
                                continue;
                        }

                        if ((cpuinfo.featureset & (1L << i)) > 0) {
                                printk(KMSG_URGENCY_LOW,
                                       "%s ", cpu_featureset[i]);
                        }
                }
                printk(KMSG_URGENCY_LOW, "\n");
        }

        printk(KMSG_URGENCY_MEDIUM,
               "Kernel finished. You are now hanging in an infinite loop. "
               "Congratulations :)");

        while (true) {}
}
