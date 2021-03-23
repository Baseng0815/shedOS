#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define IDT_TA_InterruptGate 0b10001110
#define IDT_TA_CallGate 0b10001100
#define IDT_TA_TrapGate 0b10001111

struct idt_descriptor {
        uint16_t    size;
        uintptr_t   offset;
} __attribute((packed));

struct idt_entry {
        uint16_t    offset0;
        uint16_t    selector;   /* code segment selector */
        uint8_t     ist;        /* bits 0..2 = interrupt stack table offset */
        uint8_t     type_attr;  /* type and attributes */
        uint16_t    offset1;
        uint32_t    offset2;
        uint32_t    ignore;     /* reserved */
};

void idt_initialize();

#endif
