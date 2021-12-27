/* testelf.c */

#include <stdint.h>

int global[6] = { 3, 2, 1, 6, 3, -2 };
const char *global_string = "test xdd";

int _start()
{
        /* int a = 3; */
        /* int b = 2; */
        /* int c = global[5]; */
        /* int d = global_string[3]; */
        /* /1* (3 + 2 * -2) * 116 * -1 = 116 *1/ */
        /* return (a + b * c) * d * -1; */

        uint32_t *addr = (uint32_t*)0x12345000UL;

        for (;;) {
                /* const char *msg = "TEST ELF 1 LOOP\n"; */
                char msg[3];
                msg[0] = *addr;
                msg[1] = '\n';
                msg[2] = '\0';

                asm volatile("movq %0, %%rdi;"
                             "int $0x80;"
                             : : "D" (msg));
        }

        return 12345;
}
