#include "printk.h"

#include "string.h"
#include <kernel/terminal.h>

#include <stdarg.h>

size_t utos(uint64_t, uint64_t, char*);

/* returns number of characters of the format string (%d = 1, %lu = 2...)*/
int _printk(va_list args, char fmtc)
{
        switch (fmtc) {
                case 'd': {
                        char buf[16];
                        utos(va_arg(args, int), 10, buf);
                        puts(buf);
                        return 1;
                }
                case 'x': {
                        char buf[16];
                        size_t len = utos(va_arg(args, size_t), 16, buf);
                        puts("0x");
                        /* pad with zeroes for a nicer format */
                        if (len < 8) {
                                for (size_t i = 0; i < 8 - len; i++) {
                                        putchar('0');
                                }
                        }
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

void printk(int loglevel, const char *fmt, ...)
{
        if (loglevel != KMSG_LOGLEVEL_NONE) {
                puts("[ ");
                switch (loglevel) {
                        case KMSG_LOGLEVEL_INFO:
                                /* blue */
                                terminal_setcolor(0x0000ff, 0x0);
                                puts("INFO");
                                break;
                        case KMSG_LOGLEVEL_SUCC:
                                /* green */
                                terminal_setcolor(0x00ff00, 0x0);
                                puts("SUCC");
                                break;
                        case KMSG_LOGLEVEL_WARN:
                                /* some yellowish color */
                                terminal_setcolor(0xfff000, 0x0);
                                puts("WARN");
                                break;
                        case KMSG_LOGLEVEL_CRIT:
                                /* red */
                                terminal_setcolor(0xff0000, 0x0);
                                puts("CRIT");
                                break;
                }

                terminal_setcolor(0xffffff, 0x0);
                puts(" ] ");
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

/* unsigned integer to string */
size_t utos(uint64_t i, uint64_t base, char *buf)
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

        return len;
}
