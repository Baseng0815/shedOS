#include "strutil.h"

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
