#ifndef _NVME_H
#define _NVME_H

#include "pci.h"

enum {
        NVME_CLASS      = 0x1,
        NVME_SUBCLASS   = 0x8
};

void nvme_initialize_device(struct device_table_endpoint *ep,
                            struct pci_addr pci_addr);

#endif
