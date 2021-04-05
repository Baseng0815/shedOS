#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

struct gdt_descriptor {
        uint16_t    size;
        uintptr_t   offset;
} __attribute__((packed));

struct gdt_entry {
        uint16_t    limit0;
        uint16_t    base0;
        uint8_t     base1;
        uint8_t     access;
        uint8_t     limit1_flags;
        uint8_t     base2;
} __attribute__((packed));

struct gdt {
        struct gdt_entry kernel_null;   /* @ 0x00 */
        struct gdt_entry kernel_code;   /* @ 0x08 */
        struct gdt_entry kernel_data;   /* @ 0x10 */
        struct gdt_entry user_null;
        struct gdt_entry user_code;
        struct gdt_entry user_data;
} __attribute__((packed)) __attribute__((aligned(0x1000)));

void gdt_initialize();

#endif
