#include "io.h"

void io_outb(uint16_t port, uint8_t value)
{
        asm volatile("outb %0, %1;"
                     : : "a" (value), "Nd" (port));
}

uint8_t io_inb(uint16_t port)
{
        uint8_t value;
        asm volatile("inb %1, %0;"
                     : "=a" (value)
                     : "Nd" (port));

        return value;
}

void io_wait()
{
        /* waste single I/O cycle */
        asm volatile("outb %%al, $0x80"
                     : : "a" (0));
}
