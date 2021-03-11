#include "framebuffer.h"
#include "font.h"
#include "terminal.h"

#include <printk.h>

struct bootinfo {
        struct framebuffer  *framebuffer;
        struct psf1_font    *font;
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
               "|---> glyphs @ %x\n",
               bootinfo,
               fb,
               fb->addr, fb->size / 1000, fb->width, fb->height, fb->pitch,
               bootinfo->font,
               bootinfo->font->header,
               bootinfo->font->header->mode, bootinfo->font->header->charsize,
               bootinfo->font->glyphs);

        printk(KMSG_URGENCY_LOW,
               "Term dimensions: %dx%d\n",
               term_width, term_height);

        printk(KMSG_URGENCY_MEDIUM,
               "Kernel finished. You are now hanging in an infinite loop. "
               "Congratulations :)");

        while (true) {}
}
