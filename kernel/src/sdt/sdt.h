#ifndef _SDT_H
#define _SDT_H

#include "../stivale2.h"
#include "sdt_structs.h"

extern struct madt *madt;

void sdt_initialize(struct stivale2_struct_tag_rsdp*);

#endif
