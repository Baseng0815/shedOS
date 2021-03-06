#include "framebuffer.h"
#include "font.h"
#include "terminal.h"

#include <printk.h>

struct bootinfo {
        struct framebuffer  *framebuffer;
        struct psf1_font    *font;
};

static inline void ppx(int x, int y,
                       uint32_t pixel,
                       struct framebuffer *framebuffer)
{
        *((uint32_t*)(framebuffer->addr + framebuffer->pitch * y + 4 * x))
                = pixel;
}

void _start(struct bootinfo *bootinfo)
{
        struct framebuffer *fb = bootinfo->framebuffer;

        framebuffer_initialize(fb);
        font_initialize(bootinfo->font);
        int term_width = 80;
        int term_height = 25;
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

        for (int i = 0; i < 200; i++) {
                printk(KMSG_URGENCY_MEDIUM, "Hello%d!\n", i*i);
        }

        printk(KMSG_URGENCY_MEDIUM,
               "Kernel finished. You are now hanging in an infinite loop. "
               "Congratulations :)");

        while (true) {}
}
