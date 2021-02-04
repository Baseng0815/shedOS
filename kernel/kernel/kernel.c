#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void)
{
        terminal_initialize();
        for (int i = 0; i < 28; i++) {
                printf("Hallo Nummer %i!\n", i);
        }
        printf("DUUUUUUU!");
}
