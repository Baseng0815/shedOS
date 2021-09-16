/* testelf.c */

int global[6] = { 3, 2, 1, 6, 3, -2 };
const char *global_string = "test xdd";

int _start()
{
        int a = 3;
        int b = 2;
        int c = global[5];
        int d = global_string[3];
        /* (3 + 2 * -2) * 116 * -1 = 116 */
        return (a + b * c) * d * -1;
}
