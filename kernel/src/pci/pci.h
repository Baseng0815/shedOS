#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>
#include <stdbool.h>

#define HEADER_ENDPOINT (0x0UL)

struct pci_addr {
        uint8_t bus;
        uint8_t device;
        uint8_t function;
};

struct pci_device_header {
        uint32_t vendor_id;
        uint32_t command;
        uint32_t revision_id;
        uint32_t cl_size; /* cache line size */
} __attribute__((packed));

/* PCI hdr command */
#define PHC_IO_SPACE            (1UL << 0)
#define PHC_MEM_SPACE           (1UL << 1)
#define PHC_BUS_MASTER          (1UL << 2)
#define PHC_SPEC_CYCLES         (1UL << 3)
#define PHC_MEM_WRITE_INVLD     (1UL << 4)
#define PHC_VGA_PALETTE_SNOOP   (1UL << 5)
#define PHC_PARITY_ERR_RESP     (1UL << 6)
#define PHC_serr_ENABLE         (1UL << 8)
#define PHC_FAST_B2B_ENABLE     (1UL << 9)
#define PHC_INTERRUPT_DISABLE   (1UL << 10

struct pci_cap_hdr {
        uint8_t cap_id;
        uint8_t next;
} __attribute__((packed));

/* PCI hdr status */
#define PHS_INTERRUPT_STATUS    (1UL << 3)
#define PHS_CAPABILITIES_LIST   (1UL << 4)
#define PHS_FAST_CLOCK_CAPABLE  (1UL << 5) /* 66MHz */
#define PHS_FAST_B2B_CAPABLE    (1UL << 7)
#define PHS_MASTER_DATA_PAR_ERR (1UL << 8)
#define PHS_DEVSEL_TIMING       (1UL << 9)
#define PHS_SIG_TARGET_ABORT    (1UL << 11)
#define PHS_REC_TARGET_ABORT    (1UL << 12)
#define PHS_REC_MASTER_ABORT    (1UL << 13)
#define PHS_SIG_SYSTEM_ERROR    (1UL << 14)
#define PHS_PARITY_ERROR        (1UL << 15)

/* extended capabilities pointer is at 0x100, but we don't
 * care about that right now*/
struct pci_device_endpoint {
        struct pci_device_header hdr;

        uint32_t bar0;
        uint32_t bar1;
        uint32_t bar2;
        uint32_t bar3;
        uint32_t bar4;
        uint32_t bar5;
        uint32_t cis_pointer;
        uint32_t subsystem_vendor_id;
        uint32_t xrom_base_addr;
        uint32_t capabilities_ptr;
        uint32_t int_line;
} __attribute__((packed));

void pci_init();

uint32_t *pci_conf_addr(struct pci_addr *pci_addr);

/* extended capabilities differ from normal capabilities */
bool pci_get_cap(uintptr_t cap_ptr, uintptr_t conf_base,
                 uint16_t cap_id,
                 struct pci_cap_hdr **cap_hdr);

#endif
