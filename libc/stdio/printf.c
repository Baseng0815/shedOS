#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int _printf(const char **format,
                   va_list *parameters)
{
        int written = 0;

        const char *format_begun_at = (*format)++;
        switch (**format) {
                case 'c': {
                        (*format)++;
                        char c = (char)va_arg(*parameters, int);
                        putchar(c);

                        written++;
                        break;
                }
                case 's': {
                        (*format)++;
                        const char *str = va_arg(*parameters, const char*);
                        size_t len = strlen(str);
                        for (size_t i = 0; i < len; i++) {
                                putchar(str[i]);
                        }

                        written += len;
                        break;
                }
                case 'i': {
                        (*format)++;
                        int va = va_arg(*parameters, int);

                        size_t len = 0;
                        char str[16] = {0};
                        bool negative = va < 0;

                        do {
                                str[16 - ++len] = (char)(va % 10) + '0';
                                va /= 10;
                        } while (va > 0);

                        if (negative) { str[16 - ++len] = '-'; }
                        char *str_off = str + 16 - len;

                        for (size_t i = 0; i < len; i++) {
                                putchar(str_off[i]);
                        }
                        break;
                }
                default: {
                        (*format) = format_begun_at;
                        size_t len = strlen(*format);
                        for (size_t i = 0; i < len; i++) {
                                putchar((*format)[i]);
                        }

                        written += len;
                        (*format) += len;
                        break;
                }
        }

        return written;
}

int printf(const char *__restrict format,
           ...)
{
        va_list parameters;
        va_start(parameters, format);

        int written = 0;

        while (*format != '\0') {
                /* print until format specifier is found */
                if (format[0] != '%' || format[1] == '%') {
                        if (format[0] == '%') { format++; }
                        size_t amount = 1;
                        while (format[amount]
                               && format[amount] != '%') { amount++; }
                        /* TODO set errno to EOVERFLOW */
                        for (size_t i = 0; i < amount; i++) {
                                putchar(format[i]);
                        }

                        format += amount;
                        written += amount;
                        continue;
                }

                int w = _printf(&format, &parameters);
                if (w == -1) { continue; }
                else { written += w; }
        }


        va_end(parameters);
        return written;
}
