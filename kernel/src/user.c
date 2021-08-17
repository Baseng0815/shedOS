#include "user.h"

#include "libk/printf.h"

void user_jump()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target user.\n");

        printf(KMSG_LOGLEVEL_CRIT, "Finished target user.\n");
}
