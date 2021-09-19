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

/* PIC port */
#define PP_PIC1_Command     0x20
#define PP_PIC1_Data        0x21
#define PP_PIC2_Command     0xa0
#define PP_PIC2_Data        0xa1

/* PIC command */
#define PC_EOI  0x20

/* PIC initialization command word 1 */
#define PICW1_ICW4          0x01    /* ICW4 needed                  */
#define PICW1_SINGLE        0x02    /* single cascade mode          */
#define PICW1_INTERVAL4     0x04    /* call address interval 4 (8)  */
#define PICW1_LEVEL         0x08    /* level triggered (edge) mode  */
#define PICW1_INIT          0x10    /* initialization - required    */

/* PIC initialization command word 4 */
#define PICW4_8086          0x01    /* 8086 (MSC-80/85) mode        */
#define PICW4_AUTO          0x02    /* auto EOI                     */
#define PICW4_BUF_SLAVE     0x08    /* buffered mode/slave          */
#define PICW4_BUF_MASTER    0x0c    /* buffered mode/master         */
#define PICW4_SFNM          0x10    /* special fully nested         */

/* we use the APIC so this initializes, masks and disables the PIC */
void pic_initialize();
void pic_send_eoi(bool);

#endif
