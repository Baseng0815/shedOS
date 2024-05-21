#ifndef _SDT_H
#define _SDT_H

#include "../stivale2.h"
#include "sdt_structs.h"

extern const struct madt *madt;
extern const struct hpet *hpet;
extern const struct mcfg *mcfg;

void sdt_initialize(struct stivale2_struct_tag_rsdp*);

#endif
