#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

/*
   type attribute bits:
   0-3 gate type (task, interrupt, trap)
   4 storage segment (set to 0 for interrupt and trap)
   5-6 privilege level (prevent interrupt from being called out of userspace)
   7 present (set to 0 for unused interrupts)
   */

enum idt_type_attributes {
        IDT_TA_InterruptGate    = 0b10001110,
        IDT_TA_CallGate         = 0b10001100,
        IDT_TA_TrapGate         = 0b10001111
};

struct idt_desc {
        uint16_t    offset0;
        uint16_t    selector;
        uint8_t     ist;        /* interrupt stack table offset */
        uint8_t     type_attr;
        uint16_t    offset1;
        uint32_t    offset2;
        uint32_t    reserved;
} __attribute__((packed));

struct idt {
        uint16_t size;
        uintptr_t   offset;
} __attribute__((packed));

void idt_initialize();

#endif
