#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void)
{
        terminal_initialize();
        printf("Hallo Welt\n");
        printf("%i*%i=%i", 10, 20, 10*20);
}
