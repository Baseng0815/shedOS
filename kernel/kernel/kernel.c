#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void)
{
    terminal_initialize();
    for (size_t i = 0; i < 300; i++) {
            terminal_putchar('A');
    }
}
