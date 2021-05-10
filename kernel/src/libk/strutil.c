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

int strcmp(const char *s1, const char *s2)
{
        while (*s1 != '\0' && *s2 != '\0') {
                if (*s1 < *s2) return -1;
                else if (*s1 > *s2) return 1;

                s1++;
                s2++;
        }

        return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
        for (size_t i = 0; i < n; i++) {
                if (s1[i] < s2[i]) return -1;
                else if (s1[i] > s2[i]) return 1;
                else if (s1[i] == '\0' || s2[i] == '\0') break;
        }

        return 0;
}

int strcpy(const char *src, char *tgt)
{
        return strncpy(src, strlen(src), tgt);
}

int strncpy(const char *src, size_t len, char *tgt)
{
        size_t i = 0;
        while (i < len && src[i] != '\0') {
                tgt[i] = src[i];
                i++;
        }

        return i;
}
