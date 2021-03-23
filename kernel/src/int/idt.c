#include "idt.h"

#include "interrupts.h"

#include "../memory/pfa.h"

#include <printk.h>

static struct idt_descriptor idt;

void get_offset(uintptr_t, struct idt_entry*);

void idt_initialize()
{
        printk(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        idt.size = 0x0fff;
        idt.offset = (uintptr_t)pfa_request_page();

        struct idt_entry *int_pf = (struct idt_entry*)
                (idt.offset + 0xe * sizeof(struct idt_entry));

        get_offset((uintptr_t)pf_handle, int_pf);
        int_pf->type_attr = IDT_TA_InterruptGate;
        int_pf->selector = 0x08;

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
