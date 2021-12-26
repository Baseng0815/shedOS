#ifndef _CPU_H
#define _CPU_H

void sse_enable();

void io_outb(uint16_t port, uint8_t value);
uint8_t io_inb(uint16_t port);
void io_wait();

#endif
