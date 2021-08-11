#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>
#include <stdbool.h>

enum {
        HEADER_ENDPOINT = 0x0
};

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

enum pci_hdr_command {
        IO_space            = 1U << 0,
        mem_space           = 1U << 1,
        bus_master          = 1U << 2,
        spec_cycles         = 1U << 3,
        mem_write_invld     = 1U << 4,
        vga_palette_snoop   = 1U << 5,
        parity_err_resp     = 1U << 6,
        SERR_enable         = 1U << 8,
        fast_b2b_enable     = 1U << 9,
        interrupt_disable   = 1U << 10
};

struct pci_cap_hdr {
        uint8_t cap_id;
        uint8_t next;
} __attribute__((packed));

enum pci_hdr_status {
        interrupt_status    = 1U << 3,
        capabilities_list   = 1U << 4,
        fast_clock_capable  = 1U << 5, /* 66MHz */
        fast_b2b_capable    = 1U << 7,
        master_data_par_err = 1U << 8,
        devsel_timing       = 1U << 9,
        sig_target_abort    = 1U << 11,
        rec_target_abort    = 1U << 12,
        rec_master_abort    = 1U << 13,
        sig_system_error    = 1U << 14,
        parity_error        = 1U << 15
};

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
