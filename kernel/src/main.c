#include <stdint.h>
#include <stddef.h>

#include "libk/printf.h"

#include "fb/framebuffer.h"
#include "terminal/terminal.h"

#include "memory/pfa.h"
#include "memory/paging.h"

#include "gdt/gdt.h"

static uint8_t stack[4096];

static void welcome_message();
static void dump_bootinfo(struct bootinfo*);
static void dump_cpu();
static void dump_memory(struct efi_memory_map*);

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
        /* we set the specifics to 0 to let the bootloader pick the best */
        .framebuffer_height = 0,
        .framebuffer_height = 0,
        .framebuffer_bpp    = 0
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

static void welcome_message();

void _start(struct stivale2_struct *stivale2_struct)
{
        /* framebuffer and terminal */
        struct stivale2_struct_tag_framebuffer *fb =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

        framebuffer_initialize(fb);
        int term_width = (fb->framebuffer_width - 16) / 16;
        int term_height = (fb->framebuffer_height - 16) / 16;
        terminal_initialize(term_width, term_height);

        welcome_message();

        printf(KMSG_LOGLEVEL_SUCC,
               "Framebuffer initialized with dimension of %dx%d, "
               "red mask/size %x/%x, green mask/size %x/%x, blue mask/size %x/%x "
               "and %dbpp.\n",
               fb->framebuffer_width, fb->framebuffer_height,
               fb->red_mask_shift, fb->red_mask_size,
               fb->green_mask_shift, fb->green_mask_size,
               fb->blue_mask_shift, fb->blue_mask_size,
               fb->framebuffer_bpp);

        /* memory */
        struct stivale2_struct_tag_memmap *mmap =
                stivale2_get_tag(stivale2_struct,
                                 STIVALE2_STRUCT_TAG_MEMMAP_ID);
        pfa_initialize(mmap);
        gdt_initialize();

        printf(KMSG_LOGLEVEL_INFO, "Hello Kernel!\n");

        for (;;) {
                asm("hlt");
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
