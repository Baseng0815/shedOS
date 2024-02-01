#ifndef _APIC_H
#define _APIC_H

#include "../sdt/sdt_structs.h"

void apic_initialize(const struct madt*);
void apic_send_eoi(void);

#endif
