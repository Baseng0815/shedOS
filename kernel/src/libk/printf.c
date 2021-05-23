#include "printf.h"

#include "strutil.h"
#include "../terminal/terminal.h"
#include "../interrupts/timer.h"

#include <stdarg.h>

static char buf[64];
static char timestamp_buf[16];

void _printf(va_list, char);

/* returns number of characters of the format string (%d = 1, %lu = 2...)*/
void _printf(va_list args, char fmtc)
{
        switch (fmtc) {
                case 'a': {
                        utos(va_arg(args, size_t), 16, buf, 16);
                        break;
                }
                case 'd': {
                        utos(va_arg(args, int), 10, buf, 0);
                        break;
                }
                case 'b': {
                        utos(va_arg(args, int), 2, buf, 0);
                        break;
                }
                case 'x': {
                        utos(va_arg(args, size_t), 16, buf, 0);
                        break;
                }
                case 'c': {
                        buf[0] = va_arg(args, int);
                        buf[1] = '\0';
                        break;
                }
                case 's': {
                        const char *str = va_arg(args, const char*);
                        strcpy(str, buf);
                        break;
                }
        }

        puts(buf);
}

void printf(int loglevel, const char *fmt, ...)
{
        if (loglevel != KMSG_LOGLEVEL_NONE) {
                printf(KMSG_LOGLEVEL_NONE,
                       "(%s) ", timer_format(timestamp_buf));
                puts("[");
                switch (loglevel) {
                        case KMSG_LOGLEVEL_INFO:
                                /* blue */
                                terminal_setcolor(0x0000ff, 0x0);
                                puts("INFO");
                                break;
                        case KMSG_LOGLEVEL_OKAY:
                                /* green */
                                terminal_setcolor(0x00ff00, 0x0);
                                puts("OKAY");
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
                puts("] ");
        }

        va_list args;
        va_start(args, fmt);

        while (*fmt != '\0') {
                if (*fmt == '%') {
                        char fmtc = *(++fmt);
                        _printf(args, fmtc);
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
        terminal_putchar(c);
}

void puts(const char *str)
{
        while (*str != '\0') {
                putchar(*str);
                str++;
        }
}
