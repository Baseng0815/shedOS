#include "idt.h"

#include "exceptions.h"
#include "irqs.h"

#include "../memory/pfa.h"

#include <printk.h>
#include <string.h>

static struct idt_descriptor idt;

void load_interrupt(uint8_t, uintptr_t);

void idt_initialize()
{
        printk(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        idt.size = 0x0fff;
        idt.offset = (uintptr_t)pfa_request_page();

        printk(KMSG_LOGLEVEL_INFO, "idt @ %x with size=%d, offset=%x\n",
               &idt, idt.size, idt.offset);

        /* exceptions */
        for (uint8_t i = 0; i < 21; i++) {
                load_interrupt(i, (uintptr_t)exception_interrupts[i]);
        }

        printk(KMSG_LOGLEVEL_INFO, "Loaded exception interrupts.\n");

        /* keyboard IRQ */
        load_interrupt(0x21, (uintptr_t)kb_handle);

        printk(KMSG_LOGLEVEL_INFO, "Loaded IRQs.\n");

        asm volatile("lidt %0;"
                     :
                     : "m" (idt));

        printk(KMSG_LOGLEVEL_SUCC, "Finished target idt.\n");
}

void load_interrupt(uint8_t vector, uintptr_t callback)
{
        if (callback == NULL)
                return;

        struct idt_entry *idt_entry = (struct idt_entry*)
                (idt.offset + vector * sizeof(struct idt_entry));

        idt_entry->offset0 = (callback & 0x000000000000ffff);
        idt_entry->offset1 = (callback & 0x00000000ffff0000) >> 16;
        idt_entry->offset2 = (callback & 0xffffffff00000000) >> 32;
        idt_entry->type_attr = IDT_TA_InterruptGate;
        idt_entry->selector = 0x08;
}
