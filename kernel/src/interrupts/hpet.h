#ifndef _HPET_H
#define _HPET_H

#include "../sdt/sdt_structs.h"

/* high precision event timer
   we use it to configure the local APIC timer's frequency
   the IA-PC HPET architecture defines a set of timers with a maximum of
   256 timers, each assignable to a specific task
   */

void hpet_initialize(const struct hpet *hpet);

uint64_t hpet_read_counter(void);

#endif
