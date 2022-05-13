#include "strutil.h"

#include "memutil.h"

static char *strtok_s = NULL;

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

        if (base == 16) {
                str_len += 2;
                buf[0] = '0';
                buf[1] = 'x';
        }

        buf[str_len] = '\0';
        for (size_t n = 0; n < val_len; n++) {
                char m = "0123456789ABCDEF"[i % base];
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
                uint8_t v;
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

char *strtok(char *s, char delim)
{
        if (!strtok_s) {
                if (!s)
                        return NULL;

                strtok_s = s;
        }

        char *begin = strtok_s;
        while (*strtok_s && *strtok_s != delim) {
                strtok_s++;
        }

        if (*strtok_s == '\0') {
                strtok_s = NULL;
        } else {
                *strtok_s = '\0';
                strtok_s++;
        }

        return begin;
}

int strcmp(const char *s1, const char *s2)
{
        while (*s1 && (*s1 == *s2)) {
                s1++;
                s2++;
        }

        return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
        while (*s1 && (*s1 == *s2) && --n) {
                s1++;
                s2++;
        }

        return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strcpy(char *dst, const char *src)
{
        char *ret = dst;
        while (*src != '\0') {
                *dst = *src;
                dst++;
                src++;
        }

        *dst = '\0';

        return ret;
}

char *strncpy(char *dst, const char *src, size_t n)
{
        char *ret = dst;
        while (*src != '\0' && --n) {
                *dst = *src;
                dst++;
                src++;
        }

        *dst = '\0';

        return ret;
}

size_t trim_and_terminate(char *src, size_t max_n)
{
        for (size_t i = 0; i < max_n; i++) {
                if (i == max_n - 1 || (src[i] == ' ' && src[i + 1] == ' ')) {
                        src[i] = '\0';
                        return i;
                }
        }

        return 0;
}

bool isdigit(char c)
{
        return c >= '0' && c <= '9';
}
