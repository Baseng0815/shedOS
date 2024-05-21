#include "pci.h"

#include "../sdt/sdt.h"
#include "../memory/addrutil.h"
#include "../libk/printf.h"

#include "nvme.h"

static void enumerate(uintptr_t base, struct pci_addr addr);

void pci_init()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target pci.\n");

        size_t csbaas_count = (mcfg->hdr.length - sizeof(struct mcfg))
                / sizeof(struct mcfg_csbaas);

        printf(KMSG_LOGLEVEL_INFO,
               "%d configuration space base address allocation struct(s) found,"
               " enumerating devices...\n",
               csbaas_count);

        /* we only do one for now */
        const struct mcfg_csbaas *csbaas = &mcfg->csbaas[0];
        printf(KMSG_LOGLEVEL_INFO, "base address=%a, "
               "segment group number=%d, start/end bus number=%d/%d\n",
               csbaas->base_address, csbaas->seg_group_number,
               csbaas->start_bus_number, csbaas->end_bus_number);

        /* brute-force enumeration */
        for (uint8_t bus = csbaas->start_bus_number;
             bus < csbaas->end_bus_number; bus++) {
                for (uint8_t device = 0; device < 32; device++) {
                        for (uint8_t function = 0; function < 8; function++) {
                                struct pci_addr pci_addr = {
                                        .bus        = bus,
                                        .device     = device,
                                        .function   = function
                                };

                                enumerate(csbaas->base_address, pci_addr);
                        }
                }

        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target pci.\n");
}

void enumerate(uintptr_t base, struct pci_addr pci_addr)
{
        uint32_t *conf_addr = (uint32_t*)
                addr_ensure_higher(base + (pci_addr.bus << 20 |
                                            pci_addr.device << 15 |
                                            pci_addr.function << 12));

        /* try reading the header */
        struct pci_device_header *hdr = (struct pci_device_header*)conf_addr;

        uint16_t device_id  = hdr->vendor_id >> 0x10 & 0xffff;
        uint16_t vendor_id  = hdr->vendor_id >> 0x00 & 0xffff;

        /* invalid */
        if (vendor_id == 0xffff || device_id == 0xffff)
                return;

        uint8_t rev_id      = hdr->revision_id >> 0x00 & 0xff;
        uint8_t subclass    = hdr->revision_id >> 0x10 & 0xff;
        uint8_t class       = hdr->revision_id >> 0x18 & 0xff;

        printf(KMSG_LOGLEVEL_INFO,
               "(%x:%x) %d/%d/%d: vendor:device.rev=%x:%x.%x\n",
               class, subclass, pci_addr.bus, pci_addr.device,
               pci_addr.function, vendor_id, device_id, rev_id);

        uint8_t header_type = hdr->cl_size >> 0x10 & 0xff;

        switch (header_type) {
                case HEADER_ENDPOINT: {
                        struct pci_device_endpoint *ep =
                                (struct pci_device_endpoint*)hdr;

                        uint8_t sub_vend_id =
                                ep->subsystem_vendor_id >> 0x00 & 0xffff;
                        uint8_t sub_id =
                                ep->subsystem_vendor_id >> 0x10 & 0xffff0000;
                        uint8_t int_line = ep->int_line >> 0x00 & 0xff;
                        uint8_t int_pin = (ep->int_line >> 0x08 & 0xff);

                        printf(KMSG_LOGLEVEL_INFO,
                               "endpoint: subsystem vid/id=%x/%x, "
                               "int_line=%d, " "int_pin=%d\n",
                               sub_vend_id, sub_id, int_line, int_pin);

                        if (class == NVME_CLASS &&
                            subclass == NVME_SUBCLASS) {
                                nvme_initialize_device(ep, pci_addr);
                        }
                        break;
                }

                default:
                        break;
        }
}

bool pci_get_cap(uintptr_t cap_ptr, uintptr_t conf_base,
                 uint16_t cap_id,
                 struct pci_cap_hdr **cap_hdr)
{
        for (struct pci_cap_hdr *hdr =
             (struct pci_cap_hdr*)(cap_ptr + conf_base);
             hdr != NULL; hdr = (struct pci_cap_hdr*)(hdr->next + conf_base)) {
                if (hdr->cap_id == cap_id) {
                        *cap_hdr = hdr;
                        return true;
                }
        }

        return false;
}
