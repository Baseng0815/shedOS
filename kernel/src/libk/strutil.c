#include "strutil.h"

#include "memutil.h"

size_t strlen(const char *str)
{
        size_t len = 0;
        while (str[len] != '\0') {
                len++;
        }

        return len;
}

size_t utos(uint64_t i, uint64_t base, char *buf, size_t min_len)
{
        size_t val_len = 0;
        uint64_t _i = i;
        do {
                val_len++;
        } while ((_i /= base) > 0);

        if (val_len < min_len)
                val_len = min_len;

        size_t str_len = val_len;

        if (base == 0x10) {
                str_len += 2;
                buf[0] = '0';
                buf[1] = 'x';
        }

        buf[str_len] = '\0';
        for (size_t n = 0; n < val_len; n++) {
                char m = '0' + i % base;
                if (m > '9') { m += 0x7; }
                buf[str_len - n - 1] = m;

                i /= base;
        }

        return str_len;
}

uint64_t stou(const char *str, uint64_t base)
{
        uint64_t value = 0;
        uint64_t digit = 1;
        size_t len = strlen(str);
        for (size_t i = 0; i < len; i++) {
                int v;
                char c = str[len - i - 1];
                if (c > '9') {
                        if (c > 'Z')    v = c - 'a';
                        else            v = c - 'z';
                } else {
                        v = c - '0';
                }

                if (v >= base)
                        return 0;


                value += v * digit;
                digit *= base;
        }

        return value;
}

size_t strcmp(const char *s1, const char *s2)
{
        while (*s1 != '\0' && *s2 != '\0') {
                if (*s1 < *s2) return -1;
                else if (*s1 > *s2) return 1;

                s1++;
                s2++;
        }

        return 0;
}

size_t strncmp(const char *s1, const char *s2, size_t n)
{
        for (size_t i = 0; i < n; i++) {
                if (s1[i] < s2[i]) return -1;
                else if (s1[i] > s2[i]) return 1;
                else if (s1[i] == '\0' || s2[i] == '\0') break;
        }

        return 0;
}

size_t strcpy(const char *src, char *dst)
{
        return strncpy(src, dst, strlen(src));
}

size_t strncpy(const char *src, char *dst, size_t len)
{
        size_t i = 0;
        while (i < len && src[i] != '\0') {
                dst[i] = src[i];
                i++;
        }

        dst[i] = '\0';

        return i;
}

size_t trim_and_terminate(char *src, size_t max_n)
{
        for (size_t i = 0; i < max_n; i++) {
                if (i == max_n - 1 || (src[i] == ' ' && src[i + 1] == ' ')) {
                        src[i] = '\0';
                        return i;
                }
        }
}

bool isdigit(char c)
{
        return c >= '0' && c <= '9';
}
