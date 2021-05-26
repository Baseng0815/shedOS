#include "idt.h"

#include "../libk/printf.h"

#include "../memory/pmm.h"
#include "../memory/paging.h"

#include "exceptions.h"
#include "irqs.h"

static struct idt idt;

static void load_interrupt(uint8_t, uintptr_t);

void idt_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target idt.\n");

        idt.size = 0x1000 - 1;;
        idt.offset = VADDR_ENSURE_HIGHER(pmm_request_pages(1));
        memset(idt.offset, 0, 0x1000);

        printf(KMSG_LOGLEVEL_INFO, "idt at %a with size=%d\n",
               idt.offset, idt.size);

        /* set up exceptions */
        for (uint8_t i = 0; i < 21; i++) {
                load_interrupt(i, (uintptr_t)exception_interrupts[i]);
        }

        /* hpet interrupt */
        load_interrupt(0x22, (uintptr_t)hpet_handle);

        asm volatile("lidt %0;"
                     : : "m" (idt));

        printf(KMSG_LOGLEVEL_OKAY, "Finished target idt.\n");
}

void load_interrupt(uint8_t vector,
                    uintptr_t callback)
{
        if (callback == NULL)
                return;

        struct idt_desc *desc = (struct idt_desc*)
                (idt.offset + vector * sizeof(struct idt_desc));

        desc->offset0 = (callback & 0x000000000000ffff);
        desc->offset1 = (callback & 0x00000000ffff0000) >> 16;
        desc->offset2 = (callback & 0xffffffff00000000) >> 32;
        desc->ist = 0;
        desc->type_attr = IDT_TA_InterruptGate;
        desc->selector = 0x08;
}
