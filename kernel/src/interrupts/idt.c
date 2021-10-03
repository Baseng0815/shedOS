#include "idt.h"

#include "../libk/printf.h"

#include "interrupts.h"

static struct idt_desc idt_descriptors[256] = { 0 };

static void load_interrupt(uint8_t, uintptr_t, uint8_t);

void idt_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        printf(KMSG_LOGLEVEL_INFO, "idt at %a\n", idt_descriptors);

        /* set up exceptions */
        for (uint8_t i = 0; i < 31; i++) {
                load_interrupt(i, __exception_interrupts[i], IDT_TA_Interrupt);
        }

        /* timer */
        load_interrupt(34, (uintptr_t)&__isr34, IDT_TA_Interrupt);

        /* syscall */
        load_interrupt(128, (uintptr_t)&__isr128, IDT_TA_SystemCall);

        struct idt idt = {
                .size = 0x1000 - 1,
                .offset = idt_descriptors
        };

        asm volatile("lidt %0;"
                     : : "m" (idt));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target idt.\n");
}

void load_interrupt(uint8_t vector,
                    uintptr_t callback,
                    uint8_t type_attr)
{
        if (callback == NULL)
                return;

        struct idt_desc *desc = &idt_descriptors[vector];

        desc->offset0 = callback >> 0   & 0xffff;
        desc->offset1 = callback >> 16  & 0xffff;
        desc->offset2 = callback >> 32  & 0xffffffff;
        desc->ist = 0;
        desc->type_attr = type_attr;
        desc->selector = 0x08;
}
