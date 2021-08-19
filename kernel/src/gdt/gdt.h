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

/* 16-byte version, used for TSS */
struct gdt_entry_x {
        struct gdt_entry lower_entry;
        uint32_t base3;
        uint32_t reserved0;
} __attribute__((packed));

struct tss {
        uint32_t reserved0;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserved1;
        uint64_t ist1;
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;
        uint32_t reserved2;
        uint32_t iopb_offset;
} __attribute__((packed));

struct gdt {
        struct gdt_entry null;          /* @ 0x00 */
        struct gdt_entry kernel_code;   /* @ 0x08 */
        struct gdt_entry kernel_data;   /* @ 0x10 */
        struct gdt_entry user_code;     /* @ 0x18 */
        struct gdt_entry user_data;     /* @ 0x20 */
        struct gdt_entry_x tss;         /* @ 0x28 */
} __attribute__((packed));

void gdt_initialize(void *kernel_stack);

#endif
