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
#define PIC1_Command    0x20
#define PIC1_Data       0x21
#define PIC2_Command    0xa0
#define PIC2_Data       0xa1

/* end of interrupt command code */
#define PIC_EOI         0x20

/* PIC initialization command words (ICWs)                      */
#define ICW1_ICW4       0x01    /* ICW4 needed                  */
#define ICW1_SINGLE     0x02    /* single cascade mode          */
#define ICW1_INTERVAL4  0x04    /* call address interval 4 (8)  */
#define ICW1_LEVEL      0x08    /* level triggered (edge) mode  */
#define ICW1_INIT       0x10    /* initialization - required    */

#define ICW4_8086       0x01    /* 8086 (MSC-80/85) mode        */
#define ICW4_AUTO       0x02    /* auto EOI                     */
#define ICW4_BUF_SLAVE  0x08    /* buffered mode/slave          */
#define ICW4_BUF_MASTER 0x0c    /* buffered mode/master         */
#define ICW4_SFNM       0x10    /* special fully nested         */

void pic_send_eoi(bool slave);
void pic_initialize();

#endif
