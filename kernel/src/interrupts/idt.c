#include "idt.h"

#include "../libk/printf.h"

#include "exceptions.h"
#include "irqs.h"

static struct idt_desc idt_descriptors[256] = { 0 };

static void load_interrupt(uint8_t, uintptr_t);

void idt_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        printf(KMSG_LOGLEVEL_INFO, "idt at %a\n", idt_descriptors);

        /* set up exceptions */
        for (uint8_t i = 0; i < 21; i++) {
                load_interrupt(i, (uintptr_t)exception_interrupts[i]);
        }

        /* hpet interrupt */
        load_interrupt(0x22, (uintptr_t)hpet_handle);

        struct idt idt = {
                .size = 0x1000 - 1,
                .offset = idt_descriptors
        };

        asm volatile("lidt %0;"
                     : : "m" (idt));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target idt.\n");
}

void load_interrupt(uint8_t vector,
                    uintptr_t callback)
{
        if (callback == NULL)
                return;

        struct idt_desc *desc = &idt_descriptors[vector];

        desc->offset0 = callback >> 0   & 0xffff;
        desc->offset1 = callback >> 16  & 0xffff;
        desc->offset2 = callback >> 32  & 0xffffffff;
        desc->ist = 0;
        desc->type_attr = IDT_TA_InterruptGate;
        desc->selector = 0x08;
}
