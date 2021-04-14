#include "sdt.h"

#include <stdbool.h>

#include "sdt_structs.h"
#include "../assert.h"
#include "../libk/strutil.h"

struct madt *madt;

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

        struct rsdp *rsdp = (struct rsdp*)stivale_rsdp->rsdp;
        rsdp->signature[7] = '\0';
        assert(do_checksum_rsdp(rsdp));

        printf(KMSG_LOGLEVEL_INFO,
               "rsdp signature=%s, oemid=%s, revision=%d\n",
               rsdp->signature, rsdp->oem_id, rsdp->revision);

        use_xsdt = rsdp->revision > 0;
        if (use_xsdt) {
                xsdt = (struct xsdt*)(uintptr_t)rsdp->xsdt_addr;
                assert(do_checksum_sdt(&xsdt->hdr));
                printf(KMSG_LOGLEVEL_INFO, "Using xsdt at %x\n", xsdt);
        } else {
                rsdt = (struct rsdt*)(uintptr_t)rsdp->rsdt_addr;
                assert(do_checksum_sdt(&rsdt->hdr));
                printf(KMSG_LOGLEVEL_INFO, "Using rsdt at %x\n", rsdt);
        }

        madt = (struct madt*)find_sdt("APIC");
        assert(do_checksum_sdt(&madt->hdr));

        printf(KMSG_LOGLEVEL_SUCC, "Finished target sdt.\n");
}

static bool do_checksum_rsdp(struct rsdp *rsdp)
{
        uint8_t sum = 0;

        for (size_t i = 0; i < rsdp->length; i++) {
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
                                ((uintptr_t)xsdt + sizeof(struct xsdt));
                        hdr = (struct sdt_header*)sdt_base[i];
                } else {
                        uint32_t *sdt_base = (uint32_t*)
                                ((uintptr_t)rsdt + sizeof(struct rsdt));
                        hdr = (struct sdt_header*)sdt_base[i];
                }

                if (strcmp(signature, hdr->signature) == 0) {
                        return (void*)hdr;
                }
        }

        return NULL;
}
