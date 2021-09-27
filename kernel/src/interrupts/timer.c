#include "timer.h"

#include "../sdt/sdt.h"
#include "hpet.h"

#include "../libk/printf.h"
#include "../libk/strutil.h"

const uint64_t ticks_per_second = 1000;

/* one kernel tick is equal to 1 millisecond */
uint64_t ticks = 0;

void timer_initialize()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target timer.\n");

        if (hpet) {
                hpet_initialize(hpet);
        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target timer.\n");
}

void timer_tick()
{
        ticks++;
}

char *timer_format(char *buf)
{
        /* 0000.0000 example format */
        int seconds         = ticks / ticks_per_second;
        int milliseconds    = (ticks % ticks_per_second) * 10;

        for (size_t i = 0; i < 4; i++) {
                buf[i] = ' ';
        }

        for (size_t i = 5; i < 9; i++) {
                buf[i] = '0';
        }

        /* seconds */
        char tmp[16];
        utos(seconds, 10, tmp, 0);
        strcpy(tmp, buf + 4 - strlen(tmp));

        /* milliseconds */
        utos(milliseconds, 10, tmp, 0);
        buf[4] = '.';
        strcpy(tmp, buf + 9 - strlen(tmp));

        return buf;
}
