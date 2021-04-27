#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>
#include <stdbool.h>

/*
   the PIC (programmable interrupt controller) is a (now kinda obsolete)
   chip responsible for routing and sending hardware interrupts like
   PS/2 keyboard button presses to the CPU
   its successor is the APIC (advanced programmable interrupt controller)
   */

/* I/O ports */
enum PIC_PORT {
        PIC1_Command    = 0x20,
        PIC1_Data       = 0x21,
        PIC2_Command    = 0xa0,
        PIC2_Data       = 0xa1
};

/* end of interrupt command code */
enum PIC_COMMAND {
        PIC_EOI         = 0x20
};

/* PIC initialization command words (ICWs) */
enum ICW1 {
        ICW1_ICW4       = 0x01,       /* ICW4 needed                  */
        ICW1_SINGLE     = 0x02,       /* single cascade mode          */
        ICW1_INTERVAL4  = 0x04,       /* call address interval 4 (8)  */
        ICW1_LEVEL      = 0x08,       /* level triggered (edge) mode  */
        ICW1_INIT       = 0x10,       /* initialization - required    */
};

enum ICW4 {
        ICW4_8086       = 0x01,     /* 8086 (MSC-80/85) mode        */
        ICW4_AUTO       = 0x02,     /* auto EOI                     */
        ICW4_BUF_SLAVE  = 0x08,     /* buffered mode/slave          */
        ICW4_BUF_MASTER = 0x0c,     /* buffered mode/master         */
        ICW4_SFNM       = 0x10      /* special fully nested         */
};

/* we use the APIC so this initializes, masks and disables the PIC */
void pic_initialize();
void pic_send_eoi(bool);

#endif
