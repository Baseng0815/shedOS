/* testelf.c */

int global[6] = { 2, 1, -3, 3, 2, -7 };
const char *global_string = "bruh, that's cringe";

int _start()
{
        /* int a = -3; */
        /* int b = 13; */
        /* int c = global[2]; */
        /* int d = global_string[2]; */
        /* /1* (-3 + 13 * -3) * 117 * -1 = 4914 *1/ */
        /* return (a + b * c) * d * -1; */

        for (;;) {
                const char *msg = "TEST ELF 1 LOOP\n";

                asm volatile("movq %0, %%rdi;"
                             "int $0x80;"
                             : : "D" (msg));
        }

        return 69;
}
