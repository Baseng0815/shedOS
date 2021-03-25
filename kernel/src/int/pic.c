#include "pic.h"

#include "../io.h"

#include <printk.h>

void remap_pic();
void setup_irqs();

void pic_send_eoi(bool slave) {
        if (slave) {
                io_outb(PIC2_Command, PIC_EOI);
        }

        io_outb(PIC1_Command, PIC_EOI);
}

void pic_initialize()
{
        printk(KMSG_LOGLEVEL_INFO, "Reached target pic.\n");
        remap_pic();
        setup_irqs();
        printk(KMSG_LOGLEVEL_SUCC, "Finished target pic.\n");
}

void remap_pic()
{
        printk(KMSG_LOGLEVEL_INFO, "Remapping pic...\n");

        /* get interrupt masks */
        uint8_t a1, a2;
        a1 = io_inb(PIC1_Data);
        io_wait();
        a2 = io_inb(PIC2_Data);
        io_wait();

        printk(KMSG_LOGLEVEL_INFO, "PIC interrupt masks: %b/%b\n",
               a1, a2);

        /* initialize PIC */
        io_outb(PIC1_Command, ICW1_INIT | ICW1_ICW4);
        io_wait();
        io_outb(PIC2_Command, ICW1_INIT | ICW1_ICW4);
        io_wait();

        printk(KMSG_LOGLEVEL_INFO, "Initialized PIC.\n");

        /* we need to change the PIC offset so it doesn't collide with
           CPU exception interrupts (design mistake by IBM),
           so we remap some stuff
           */

        /* ICW2: vector offset */
        io_outb(PIC1_Data, 0x20);
        io_wait();
        io_outb(PIC2_Data, 0x28);
        io_wait();
        printk(KMSG_LOGLEVEL_INFO, "PIC ICW2: vector offsets (%x/%x).\n",
               0x20, 0x28);

        /* ICW3: master/slave wiring */
        io_outb(PIC1_Data, 4); /* tell master PIC: slave pic at IRQ2 (0100) */
        io_wait();
        io_outb(PIC2_Data, 2); /* tell slave PIC: cascade identity 2 (0010) */
        io_wait();
        printk(KMSG_LOGLEVEL_INFO, "PIC ICW3: wiring (%d/%d).\n",
               4, 2);

        /* ICW4: additional information */
        io_outb(PIC1_Data, ICW4_8086);
        io_wait();
        io_outb(PIC2_Data, ICW4_8086);
        io_wait();
        printk(KMSG_LOGLEVEL_INFO, "PIC ICW4: use ICW4_8086.\n");

        /* restore bit mask */
        io_outb(PIC1_Data, a1);
        io_wait();
        io_outb(PIC2_Data, a2);
        io_wait();
        printk(KMSG_LOGLEVEL_INFO, "PIC bit mask restored.\n");

        printk(KMSG_LOGLEVEL_SUCC, "Remapped pic.\n");
}

void setup_irqs()
{
        printk(KMSG_LOGLEVEL_INFO, "Setting up IRQs...\n");

        /* unmask keyboard interrupt */
        io_outb(PIC1_Data, 0b11111101);
        io_outb(PIC2_Data, 0b11111111);

        /* enable maskable interrupts */
        asm volatile("sti;");

        printk(KMSG_LOGLEVEL_SUCC, "IRQs set up.\n");
}
