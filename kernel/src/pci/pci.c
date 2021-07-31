#include "pci.h"

#include "../sdt/sdt.h"
#include "../memory/addrutil.h"
#include "../libk/printf.h"

/* don't use as pointer cause we need to keep reads 4-byte aligned */
struct device_header_common {
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

void pci_init()
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target pci.\n");

        size_t csbaas_count = (mcfg->hdr.length - sizeof(struct mcfg))
                / sizeof(struct mcfg_csbaas);

        printf(KMSG_LOGLEVEL_CRIT,
               "%d configuration space base address allocation struct(s) found:\n",
               csbaas_count);

        for (size_t i = 0; i < csbaas_count; i++) {
                struct mcfg_csbaas *csbaas = &mcfg->csbaas[i];
                printf(KMSG_LOGLEVEL_NONE, "|-> %d: base address=%a,"
                       "segment group number=%d, start/end bus number=%d/%d\n",
                       i, csbaas->base_address, csbaas->seg_group_number,
                       csbaas->start_bus_number, csbaas->end_bus_number);

                uint32_t *cs_regs =
                        (uint32_t*)vaddr_ensure_higher(csbaas->base_address);
                struct device_header_common hdr;

                uint32_t v = cs_regs[0];
                hdr.vendor_id       = (v & 0x0000ffff);
                hdr.device_id       = (v & 0xffff0000) >> 0x10;
                v = cs_regs[1];
                hdr.command         = (v & 0x0000ffff);
                hdr.status          = (v & 0xffff0000) >> 0x10;
                v = cs_regs[2];
                hdr.revision_id     = (v & 0x000000ff);
                hdr.prog_if         = (v & 0x0000ff00) >> 0x08;
                hdr.subclass        = (v & 0x00ff0000) >> 0x10;
                hdr.class           = (v & 0xff000000) >> 0x18;
                v = cs_regs[3];
                hdr.cl_size         = (v & 0x000000ff);
                hdr.latency_timer   = (v & 0x0000ff00) >> 0x08;
                hdr.header_type     = (v & 0x00ff0000) >> 0x10;
                hdr.bist            = (v & 0xff000000) >> 0x18;

                printf(KMSG_LOGLEVEL_INFO, "vendor_id=%x, device_id=%x, "
                       "command=%x, status=%x, revision_id=%x, prog_if=%x, "
                       "subclass=%x, class=%x, cl_size=%x, latency_timer=%x, "
                       "header_type=%x, bist=%x\n",
                       hdr.vendor_id, hdr.device_id, hdr.command, hdr.status,
                       hdr.revision_id, hdr.prog_if, hdr.subclass, hdr.class,
                       hdr.cl_size, hdr.latency_timer, hdr.header_type,
                       hdr.bist);
        }

        printf(KMSG_LOGLEVEL_OKAY, "Finished target pci.\n");
}
