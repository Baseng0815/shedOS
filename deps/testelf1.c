/* testelf.c */

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

        for (;;) {
                const char *msg = "TEST ELF 1 LOOP\n";

                asm volatile("movq %0, %%rdi;"
                             "int $0x80;"
                             : : "D" (msg));
        }
}
