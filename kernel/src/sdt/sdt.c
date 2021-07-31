#include "sdt.h"

#include <stdbool.h>

#include "sdt_structs.h"
#include "../debug.h"

#include "../memory/addrutil.h"
#include "../libk/printf.h"
#include "../libk/strutil.h"

struct madt *madt;
struct hpet *hpet;
struct mcfg *mcfg;

static bool use_xsdt;
static struct rsdt *rsdt;
static struct xsdt *xsdt;

static bool do_checksum_rsdp(struct rsdp*);
static bool do_checksum_sdt(struct sdt_header*);

static void *find_sdt(const char*);
static void dump_madt(struct madt*);

void sdt_initialize(struct stivale2_struct_tag_rsdp *stivale_rsdp)
{
        printf(KMSG_LOGLEVEL_INFO, "Reached target sdt.\n");

        printf(KMSG_LOGLEVEL_INFO, "rsdp is at %a\n", stivale_rsdp->rsdp);
        struct rsdp *rsdp = (struct rsdp*)stivale_rsdp->rsdp;
        use_xsdt = rsdp->revision > 0;

        assert(do_checksum_rsdp(rsdp),
               "RSDP checksum invalid.");

        printf(KMSG_LOGLEVEL_INFO,
               "rsdp signature=%s, oemid=%s, revision=%d\n",
               rsdp->signature, rsdp->oem_id, rsdp->revision);

        if (use_xsdt) {
                xsdt = (struct xsdt*)
                        vaddr_ensure_higher((uintptr_t)rsdp->rev1.xsdt_addr);
                assert(do_checksum_sdt(&xsdt->hdr),
                       "XSDT checksum invalid.");
                printf(KMSG_LOGLEVEL_INFO, "Using xsdt at %a\n", xsdt);
        } else {
                rsdt = (struct rsdt*)
                        vaddr_ensure_higher((uintptr_t)rsdp->rsdt_addr);
                assert(do_checksum_sdt(&rsdt->hdr),
                       "RSDT checksum invalid.");
                printf(KMSG_LOGLEVEL_INFO, "Using rsdt at %a\n", rsdt);
        }

        /* MADT */
        madt = (struct madt*)find_sdt("APIC");
        assert(madt != NULL, "MADT not present.");
        assert(madt && do_checksum_sdt(&madt->hdr), "MADT checksum invalid.");
        printf(KMSG_LOGLEVEL_INFO, "MADT at %a\n", madt);

        /* HPET */
        hpet = (struct hpet*)find_sdt("HPET");
        assert(hpet != NULL, "HPET not present.");
        assert(do_checksum_sdt(&hpet->hdr), "HPET checksum invalid.");
        printf(KMSG_LOGLEVEL_INFO, "HPET at %a\n", hpet);

        /* MCFG */
        mcfg = (struct mcfg*)find_sdt("MCFG");
        assert(mcfg != NULL, "MCFG not present.");
        assert(do_checksum_sdt(&mcfg->hdr), "MCFG checksum invalid.");
        printf(KMSG_LOGLEVEL_INFO, "MCFG at %a\n", mcfg);

        printf(KMSG_LOGLEVEL_OKAY, "Finished target sdt.\n");
}

static bool do_checksum_rsdp(struct rsdp *rsdp)
{
        uint8_t sum = 0;
        size_t len;

        if (use_xsdt) {
                len = rsdp->rev1.length;
        } else {
                len = sizeof(struct rsdp) - sizeof(struct rsdp_rev1);
        }

        for (size_t i = 0; i < len; i++) {
                sum += ((uint8_t*)rsdp)[i];
        }

        return sum == 0;
}

bool do_checksum_sdt(struct sdt_header *hdr)
{
        uint8_t sum = 0;
        for (size_t i = 0; i < hdr->length; i++) {
                sum += ((uint8_t*)hdr)[i];
        }

        return sum == 0;
}

void *find_sdt(const char *signature)
{
        size_t num_sdts;

        if (use_xsdt) {
                num_sdts = (xsdt->hdr.length - sizeof(xsdt->hdr)) / 8;
        } else {
                num_sdts = (rsdt->hdr.length - sizeof(rsdt->hdr)) / 4;
        }

        for (size_t i = 0; i < num_sdts; i++) {
                struct sdt_header *hdr;
                if (use_xsdt) {
                        uint64_t *sdt_base = (uint64_t*)
                                vaddr_ensure_higher(((uintptr_t)xsdt +
                                                     sizeof(struct xsdt)));
                        hdr = (struct sdt_header*)sdt_base[i];
                } else {
                        uint32_t *sdt_base = (uint32_t*)
                                vaddr_ensure_higher(((uintptr_t)rsdt +
                                                     sizeof(struct rsdt)));
                        hdr = (struct sdt_header*)
                                vaddr_ensure_higher(sdt_base[i]);
                }

                if (strcmp(signature, hdr->signature) == 0) {
                        return (void*)hdr;
                }
        }

        return NULL;
}
