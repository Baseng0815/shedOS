#include "idt.h"

#include "exceptions.h"

#include "../memory/pfa.h"

#include <printk.h>

static struct idt_descriptor idt;

void get_offset(uintptr_t, struct idt_entry*);
void load_exceptions();

void idt_initialize()
{
        printk(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        idt.size = 0x0fff;
        idt.offset = (uintptr_t)pfa_request_page();

        printk(KMSG_LOGLEVEL_INFO, "idt @ %x with size=%d, offset=%x\n",
               &idt, idt.size, idt.offset);

        load_exceptions();

        asm volatile("lidt %0;"
                     :
                     : "m" (idt));

        printk(KMSG_LOGLEVEL_SUCC, "Finished target idt.\n");
}

void get_offset(uintptr_t offset,
                struct idt_entry *idt_entry)
{
        idt_entry->offset0 = (offset & 0x000000000000ffff);
        idt_entry->offset1 = (offset & 0x00000000ffff0000) >> 16;
        idt_entry->offset2 = (offset & 0xffffffff00000000) >> 32;
}

void load_exceptions()
{
        for (uint8_t i = 0; i < 21; i++) {
                if (exception_interrupts[i] == NULL)
                        continue;

                struct idt_entry *idt_entry = (struct idt_entry*)
                        (idt.offset + i * sizeof(struct idt_entry));

                get_offset((uintptr_t)exception_interrupts[i], idt_entry);
                idt_entry->type_attr = IDT_TA_InterruptGate;
                idt_entry->selector = 0x08;
        }
}
