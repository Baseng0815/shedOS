#include "printk.h"

#include "string.h"
#include <kernel/terminal.h>

#include <stdarg.h>

/* unsigned integer to string */
void _utos(uint64_t i, uint64_t base, char *buf)
{
        size_t len = 0;
        uint64_t _i = i;
        do {
                len++;
        } while ((_i /= base) > 0);

        buf[len] = '\0';
        for (size_t n = 0; n < len; n++) {
                char m = '0' + i % base;
                if (m > '9') { m += 0x7; }
                buf[len - n - 1] = m;

                i /= base;
        }
}

/* returns number of characters of the format string (%d = 1, %lu = 2...)*/
int _printk(va_list args, char fmtc)
{
        switch (fmtc) {
                case 'd': {
                        char buf[16];
                        _utos(va_arg(args, int), 10, buf);
                        puts(buf);
                        return 1;
                }
                case 'x': {
                        char buf[16];
                        _utos(va_arg(args, size_t), 16, buf);
                        puts("0x");
                        puts(buf);
                        return 1;
                }

                case 's': {
                        const char *str = va_arg(args, const char*);
                        puts(str);
                        return 1;
                }
        }

        return 0;
}

void printk(int urgency, const char *fmt, ...)
{
        switch (urgency) {
                case KMSG_URGENCY_LOW:
                        terminal_setcolor(0x00ff00, 0x0);
                        break;
                case KMSG_URGENCY_MEDIUM:
                        terminal_setcolor(0xfff000, 0x0);
                        break;
                case KMSG_URGENCY_HIGH:
                        terminal_setcolor(0x0000ff, 0x0);
                        break;
        }

        va_list args;
        va_start(args, fmt);

        while (*fmt != '\0') {
                if (*fmt == '%') {
                        char fmtc = *(fmt + 1);
                        fmt += _printk(args, fmtc);
                } else {
                        putchar(*fmt);
                }

                if (*fmt != '\0') { fmt++; }
        }

        /* restore default color */
        terminal_setcolor(0xffffff, 0x0);
        va_end(args);
}

void putchar(char c)
{
#ifdef __IS_LIBK
        terminal_putchar(c);
#endif
}

void puts(const char *str)
{
        while (*str != '\0') {
                putchar(*str);
                str++;
        }
}
