#include "pci.h"

#include "../sdt/sdt.h"
#include "../memory/addrutil.h"
#include "../libk/printf.h"

enum {
        HEADER_ENDPOINT = 0x0
};

/* don't use as pointer cause we need to keep reads 4-byte aligned */
struct device_header {
        uint16_t vendor_id;
        uint16_t device_id;
        uint16_t command;
        uint16_t status;
        uint8_t revision_id;
        uint8_t prog_if;
        uint8_t subclass;
        uint8_t class;
        uint8_t cl_size; /* cache line size */
        uint8_t latency_timer;
        uint8_t header_type;
        uint8_t bist;
};

struct device_table_endpoint {
        struct device_header hdr;

        uint32_t bar0;
        uint32_t bar1;
        uint32_t bar2;
        uint32_t bar3;
        uint32_t bar4;
        uint32_t bar5;
        uint32_t cis_pointer;
        uint16_t subsystem_vendor_id;
        uint16_t subsystem_id;
        uint32_t xrom_base_addr;
        uint8_t capabilities_ptr;
        uint8_t int_line;
        uint8_t int_pin;
        uint8_t min_grant;
        uint8_t max_latency;
};

static void enumerate(uintptr_t base,
                      uint8_t bus,
                      uint8_t device,
                      uint8_t function);

void read_table_endpoint(uint32_t *base, struct device_table_endpoint *ep);

void pci_init()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target pci.\n");

        size_t csbaas_count = (mcfg->hdr.length - sizeof(struct mcfg))
                / sizeof(struct mcfg_csbaas);

        printf(KMSG_LOGLEVEL_INFO,
               "%d configuration space base address allocation struct(s) found:\n",
               csbaas_count);

        /* we only do one for now */
        struct mcfg_csbaas *csbaas = &mcfg->csbaas[0];
        printf(KMSG_LOGLEVEL_INFO, "csbaas: base address=%a, "
               "segment group number=%d, start/end bus number=%d/%d\n",
               csbaas->base_address, csbaas->seg_group_number,
               csbaas->start_bus_number, csbaas->end_bus_number);

        /* brute-force enumeration */
        for (uint8_t bus = csbaas->start_bus_number;
             bus < csbaas->end_bus_number; bus++) {
                for (uint8_t device = 0; device < 32; device++) {
                        for (uint8_t function = 0; function < 8; function++) {
                                enumerate(csbaas->base_address,
                                          bus, device, function);
                        }
                }

        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target pci.\n");
}

void read_header(uint32_t *base, struct device_header *hdr)
{
        base = vaddr_ensure_higher(base);
        uint32_t v = base[0];
        hdr->vendor_id       = (v & 0x0000ffff);
        hdr->device_id       = (v & 0xffff0000) >> 0x10;
        v = base[1];
        hdr->command         = (v & 0x0000ffff);
        hdr->status          = (v & 0xffff0000) >> 0x10;
        v = base[2];
        hdr->revision_id     = (v & 0x000000ff);
        hdr->prog_if         = (v & 0x0000ff00) >> 0x08;
        hdr->subclass        = (v & 0x00ff0000) >> 0x10;
        hdr->class           = (v & 0xff000000) >> 0x18;
        v = base[3];
        hdr->cl_size         = (v & 0x000000ff);
        hdr->latency_timer   = (v & 0x0000ff00) >> 0x08;
        hdr->header_type     = (v & 0x00ff0000) >> 0x10;
        hdr->bist            = (v & 0xff000000) >> 0x18;
}

void read_table_endpoint(uint32_t *base, struct device_table_endpoint *ep)
{
        base = (uint32_t*)vaddr_ensure_higher((uintptr_t)base + 16);
        ep->bar0                = base[0];
        ep->bar1                = base[1];
        ep->bar2                = base[2];
        ep->bar3                = base[3];
        ep->bar4                = base[4];
        ep->bar5                = base[5];
        ep->cis_pointer         = base[6];
        uint32_t v = base[7];
        ep->subsystem_vendor_id = (v & 0x0000ffff);
        ep->subsystem_id        = (v & 0xffff0000) >> 0x10;
        ep->xrom_base_addr      = base[8];
        v = base[9];
        ep->capabilities_ptr    = (v & 0x000000ff);
        v = base[11];
        ep->int_line            = (v & 0x000000ff);
        ep->int_pin             = (v & 0x0000ff00) >> 0x08;
        ep->min_grant           = (v & 0x00ff0000) >> 0x10;
        ep->max_latency         = (v & 0xff000000) >> 0x18;
}

void enumerate(uintptr_t base,
               uint8_t bus,
               uint8_t device,
               uint8_t function)
{
        uint32_t *conf_addr = (uint32_t*)vaddr_ensure_higher(base +
                                                             (bus << 20 | device << 15 | function << 12));

        /* try reading a header */
        struct device_header hdr;
        read_header(conf_addr, &hdr);

        if (hdr.vendor_id == 0xffff)
                return;

        printf(KMSG_LOGLEVEL_INFO,
               "(%x:%x) %d/%d/%d: vendor:device=%x:%x, command=%d, status=%d, "
               "revision_id=%d, prog_if=%d, " "cl_size=%x, latency_timer=%d, "
               "header_type=%x, bist=%d\n",
               hdr.class, hdr.subclass, bus, device, function, hdr.vendor_id,
               hdr.device_id, hdr.command, hdr.status, hdr.revision_id,
               hdr.prog_if, hdr.cl_size, hdr.latency_timer, hdr.header_type,
               hdr.bist);

        switch (hdr.header_type) {
                case HEADER_ENDPOINT: {
                        struct device_table_endpoint ep;
                        read_table_endpoint(base, &ep);
                        printf(KMSG_LOGLEVEL_NONE,
                               "|-> ENDPOINT: subsystem vid/id=%x/%x, "
                               "int_line=%d, " "int_pin=%d, min_grant=%d, "
                               "max_latency=%d\n",
                               ep.subsystem_vendor_id, ep.subsystem_id,
                               ep.int_line, ep.int_pin,
                               ep.min_grant, ep.max_latency);
                        break;
                }

                default:
                        break;
        }
}
