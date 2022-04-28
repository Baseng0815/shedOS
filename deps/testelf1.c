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
        if (fork() == 0) {
                for (;;) {
                        for (int i = 0; i < 10000000; i++);
                        print("child!\n");
                }
        } else {
                for (;;) {
                        for (int i = 0; i < 10000000; i++);
                        print("parent!\n");
                }
        }
}
