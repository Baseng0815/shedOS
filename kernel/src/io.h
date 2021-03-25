#ifndef _IO_H
#define _IO_H

#include <stdint.h>

void io_outb(uint16_t, uint8_t);
uint8_t io_inb(uint16_t);
void io_wait();

#endif
