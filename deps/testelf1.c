/* testelf.c */

#include <stdint.h>

int fork()
{
        int id;
        asm volatile("movq $57, %%rax;"
                     "int $0x80;"
                     : "=a" (id));
        return id;
}

void print(const char *str)
{

        asm volatile("movq $2, %%rax;"
                     "movq %0, %%rdi;"
                     "int $0x80;"
                     : : "D" (str));
}

int _start()
{
        int ret = fork();
        for (;;) {
                char str[] = { ret+'0', '\n', '\0' };
                print(str);
        }

        return 12345;
}
