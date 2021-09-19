#ifndef _NVME_H
#define _NVME_H

#include "pci.h"

#define NVME_CLASS      0x1
#define NVME_SUBCLASS   0x8

void nvme_initialize_device(struct pci_device_endpoint *ep,
                            struct pci_addr pci_addr);

#endif
