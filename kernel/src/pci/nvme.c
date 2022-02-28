#include "pci.h"

#include "../memory/addrutil.h"
#include "../memory/paging.h"
#include "../memory/vmm.h"
#include "../memory/pmm.h"
#include "../debug.h"

#include "../libk/printf.h"
#include "../libk/strutil.h"

struct regs {
        uint64_t cap;
        uint32_t vs;
        uint32_t intms;
        uint32_t intmc;
        uint32_t cc;
        uint32_t reserved0;
        uint32_t csts;
        uint32_t nssr;
        uint32_t aqa;
        uint64_t asq;
        uint64_t acq;
        uint32_t cmbloc;
        uint32_t cmbsz;
} __attribute__((packed));

struct sq_entry {
        uint32_t cdw0;
        uint32_t nsid; /* cdw 1 */
        uint32_t reserved0[2];
        uint64_t mptr; /* cdw 4/5 */
        uint64_t prp1; /* cdw 6/7 */
        uint64_t prp2; /* cdw 8/9 */
        uint32_t cdw10;
        uint32_t cdw11;
        uint32_t cdw12;
        uint32_t cdw13;
        uint32_t cdw14;
        uint32_t cdw15;
} __attribute__((packed));

struct cq_entry {
        uint32_t cmd_specific;
        uint32_t reserved;
        uint16_t sq_head_ptr;
        uint16_t sq_identifier;
        uint16_t cmd_identifier;
        uint16_t status_field;
} __attribute__((packed));

struct queue {
        int id;
        uint32_t sq_tail, cq_head;
        struct sq_entry *sq;
        struct cq_entry *cq;
        uint32_t *sq_tail_dbl;
        uint32_t *cq_head_dbl;
};

struct msix_cap {
        struct pci_cap_hdr hdr;
        uint16_t mxc; /* message control */
        uint32_t mtab; /* table offset and table BIR */
        uint32_t mpba; /* PBA offset and PBA BIR */
} __attribute__((packed));

struct msix_table_entry {
        uint32_t lower_addr;
        uint32_t upper_addr;
        uint32_t msg_data;
        uint32_t vec_control;
} __attribute__((packed));

struct ctrl_id {
        uint16_t vid; /* PCI vendor ID */
        uint16_t ssvid; /* PCI subsystem vendor ID */
        char sn[20]; /* serial number */
        char mn[40]; /* model number */
        char fr[8]; /* firmware revision */
        uint8_t rab; /* recommended arbitration burst size */
        char ieee[3]; /* IEEE OUI identifier */
        uint8_t cmic; /* controller MP I/O and namespace sharing capabilities */
        uint8_t mdts; /* maximum data transfer size */
        uint16_t cntlid; /* controller id */
        uint32_t ver; /* version */
        uint32_t rtd3r; /* RTD3 resume latency */
        uint32_t rtd3e; /* RTD3 entry latency */
        uint32_t oaes; /* optional asynchronous events supported */
        uint32_t ctratt; /* controller attributes */
        uint16_t rrls; /* read recovery levels supported */
        uint8_t reserved0[9];
        uint8_t cntrltype; /* controller type */
        char fguid[16]; /* FRU globally unique identifier */
        uint16_t crdt1; /* command retry delay time 1 */
        uint16_t crdt2; /* command retry delay time 2 */
        uint16_t crdt3; /* command retry delay time 3 */
        uint8_t reserved1[119];
        uint8_t nvmsr; /* NVM subsystem report */
        uint8_t vwci; /* vpd write cycle information */
        uint8_t mec; /* management endpoint capabilities */
        uint16_t oacs; /* optional admin command support */
        uint8_t acl; /* abort command limit */
        uint8_t aerl; /* asynchronous event request limit */
        uint8_t frmw; /* firmware updates */
        uint8_t lpa; /* log page attributes */
        uint8_t elpe; /* error log page entries */
        uint8_t npss; /* number of power states support */
        uint8_t avscc; /* admin vendor specific command configuration */
        uint8_t apsta; /* autonomous power state transition attributes */
        uint16_t wctemp; /* warning composite temperature threshold */
        uint16_t cctemp; /* critical composite temperature threshold */
        uint16_t mtfa; /* maximum time for firmware activation */
        uint32_t hmpre; /* host memory buffer preferred size */
        uint32_t hmmin; /* host memory buffer minimum size */
        uint64_t tnvmcap[2]; /* total NVM capacity */
        uint64_t unvmcap[2]; /* unallocated NVM capacity */
        uint32_t rpmbs; /* replay protected memory block support */
        /* I will finish this maybe some time later */
} __attribute__((packed));

struct lba_format {
        uint16_t ms; /* metadata size */
        uint8_t lbads; /* LBA data size */
        uint8_t rp; /* relative performance */
} __attribute__((packed));

struct namespace_id {
        uint64_t nsze; /* namespace size */
        uint64_t ncap; /* namespace capacity */
        uint64_t nuse; /* namespace utilization */
        uint8_t nsfeat; /* namespace features */
        uint8_t nlbaf; /* number of LBA features */
        uint8_t flbas; /* formatted LBA size */
        uint8_t mc; /* metadata capabilities */
        uint8_t dpc; /* end-to-end data protection capabilities */
        uint8_t dps; /* end-to-end data type settings */
        uint8_t nmic; /* namespace multi-path I/O and sharing capabilities */
        uint8_t rescap; /* reservation capabilities */
        uint8_t fpi; /* format progress indicator */
        uint8_t reserved0;
        uint16_t nawun; /* namespace atomic write unit normal */
        uint16_t nawupf; /* namespace atomic write unit power fail */
        uint16_t nacwu; /* namespace atomic compare & write unit */
        uint16_t nabsn; /* namespace atomic boundary size normal */
        uint16_t nabo; /* namespace atomic boundary offset */
        uint16_t nabspf; /* namespace atomic boundary size power fail */
        uint16_t reserved1;
        uint8_t nvmcap[16]; /* NVM capacity */
        uint8_t reserved2[40];
        uint64_t nguid[2]; /* namespace globally unique identifier */
        uint64_t eui64; /* IEEE extended unique number */
        struct lba_format lba_formats[16];
        uint8_t reserved3[3904];
} __attribute__((packed));

struct regs *regs;
struct queue admin_queue;
struct queue io_queue;

struct ctrl_id *ctrl_id; /* ctrl identify data structure */
size_t nsid_count;
uint32_t *nsid; /* namespace IDs */
struct namespace_id *namespace_id; /* first namespace identify data structure */

static void create_admin_queue(void);
static void configure_msix(struct pci_device_endpoint *ep);
static void create_io_queue(void);
static void namespace_identify(void);
static void ctrl_identify(void);

static void send_cmd_sync(struct queue *queue,
                          const struct sq_entry *command,
                          struct cq_entry *result);

void nvme_initialize_device(struct pci_device_endpoint *ep,
                            struct pci_addr pci_addr)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing NVME device (%x/%x/%x)...\n",
               pci_addr.bus, pci_addr.device, pci_addr.function);

        /* make bus master and enable mmio */
        ep->hdr.command |= PHC_BUS_MASTER;
        ep->hdr.command |= PHC_MEM_SPACE;
        ep->hdr.command |= PHC_IO_SPACE;

        assert((ep->bar0 & 0x4) > 0,
               "NVMe BAR does not support 64-bit mapping.");

        /* find out size of BAR address space */
        const uint32_t prev_bar = ep->bar0;
        ep->bar0 = 0xffffffff;
        const uint32_t cur_bar = ep->bar0 & 0xfffff000;
        ep->bar0 = prev_bar;

        const size_t bar_size = ~cur_bar + 1;
        regs = (struct regs*)addr_ensure_higher(ep->bar0);
        printf(KMSG_LOGLEVEL_INFO, "BAR=%a, size=%x bytes\n",
               regs, bar_size);

        /* assert capabilities (NVM, page size) */
        assert((regs->cap >> 37 & 0x1) > 0, "NVM command set not supported.");

        const uint32_t page_size_min = 1 << (12 + (regs->cap >> 48 & 0xf));
        const uint32_t page_size_max = 1 << (12 + (regs->cap >> 52 & 0xf));
        assert(page_size_max >= 0x1000 && page_size_min <= 0x1000,
               "The NVM controller can't handle pages of this size.");

        /* version */
        const uint8_t     tert_version    = regs->vs >> 0  & 0xff;
        const uint8_t     minor_version   = regs->vs >> 8  & 0xff;
        const uint16_t    major_version   = regs->vs >> 16 & 0xffff;
        printf(KMSG_LOGLEVEL_INFO, "NVMe version %d.%d.%d\n",
               major_version, minor_version, tert_version);

        /* disable controller */
        regs->cc &= ~(1 << 0);

        /* wait for controller to be disabled */
        while (regs->csts & 1 << 0) {}

        create_admin_queue();

        /* reenable controller */
        regs->cc =
                1 << 0  | /* enable */
                0 << 4  | /* NVM command set */
                0 << 7  | /* 2^(12 + 0) = 4K page size */
                0 << 11 | /* round robin */
                0 << 14 | /* no notification */
                6 << 16 | /* 2^6 = 64 byte sqentry size */
                4 << 20;  /* 2^4 = 16 byte cqentry size */

        while (!(regs->csts & 1 << 0)) {}

        /* MSI-X */
        configure_msix(ep);

        create_io_queue();

        printf(KMSG_LOGLEVEL_INFO, "Controller configured and reenabled.\n");

        ctrl_identify();
        namespace_identify();

        uint8_t *md = (uint8_t*)
                addr_ensure_higher((uint64_t)pmm_request_pages(1));
        uint8_t *data = (uint8_t*)
                addr_ensure_higher((uint64_t)pmm_request_pages(1));

        /* test read */
        struct sq_entry cmd = {
                .cdw0 = 0x2,
                .nsid = nsid[0],
                .mptr = addr_ensure_lower((uint64_t)md),
                .prp1 = addr_ensure_lower((uint64_t)data),
                .cdw10 = 0x44a000 / 512, .cdw11 = 0, /* starting LBA */
                .cdw12 = 0 /* number of LBAs - 1 */
        };
        send_cmd_sync(&io_queue, &cmd, NULL);

        /* dump 256 qwords */
        for (size_t i = 0; i < 16; i++) {
                for (size_t j = 0; j < 16; j++)
                        printf(KMSG_LOGLEVEL_NONE, "%x ", data[i * 16 + j]);
                printf(KMSG_LOGLEVEL_NONE, "\n");
        }
}

void create_admin_queue(void)
{
        /* AQA, ASQ, ACQ (256 entries each) */
        const size_t asq_page_count = sizeof(struct sq_entry) * 256 / 0x1000;
        const size_t acq_page_count = sizeof(struct cq_entry) * 256 / 0x1000;
        regs->asq = (uint64_t)pmm_request_pages(asq_page_count);
        regs->acq = (uint64_t)pmm_request_pages(acq_page_count);
        regs->aqa =
                255 << 0 | /* submission queue entries */
                255 << 16; /* completion queue entries */

        /* configure admin queue */
        admin_queue.sq = (struct sq_entry*)addr_ensure_higher(regs->asq);
        admin_queue.cq = (struct cq_entry*)addr_ensure_higher(regs->acq);
        admin_queue.sq_tail = 0;
        admin_queue.cq_head = 0;

        const uint32_t doorbell_stride = 1 << (2 + (regs->cap >> 32 & 0xf));
        admin_queue.sq_tail_dbl = (uint32_t*)
                ((uint64_t)regs + 0x1000 + (2 * 0 + 0) * doorbell_stride);
        admin_queue.cq_head_dbl = (uint32_t*)
                ((uint64_t)regs + 0x1000 + (2 * 0 + 1) * doorbell_stride);

        printf(KMSG_LOGLEVEL_INFO,
               "ASQ/ACQ at %x/%x, AQA=%x, doorbell stride=%x\n",
               regs->asq, regs->acq, regs->aqa, doorbell_stride);
}

void configure_msix(struct pci_device_endpoint *ep)
{
        /* find capability */
        struct msix_cap *msix_cap;
        assert(pci_get_cap(ep->capabilities_ptr, (uint64_t)ep,
                           0x11,
                           (struct pci_cap_hdr**)&msix_cap),
               "MSI-X capability not found.");
        printf(KMSG_LOGLEVEL_INFO, "MSI-X: mxc=%x, mpba=%x, mtab=%x\n",
               msix_cap->mxc, msix_cap->mpba, msix_cap->mtab);

        size_t table_size = (msix_cap->mxc & 0x7ff) + 1;

        /* get table and pba address */
        uint64_t table_addr, pba_addr;
        switch (msix_cap->mtab & 0x7) {
                case 0: table_addr = ep->bar0; break;
                case 4: table_addr = ep->bar4; break;
                case 5: table_addr = ep->bar5; break;
                default:
                        kernel_panic("Table BIR invalid.", __FILE__, __LINE__);
        }

        struct msix_table_entry *table = (struct msix_table_entry*)
                addr_ensure_higher(table_addr + (msix_cap->mtab & ~0x7));

        switch (msix_cap->mpba & 0x7) {
                case 0: pba_addr = ep->bar0; break;
                case 4: pba_addr = ep->bar4; break;
                case 5: pba_addr = ep->bar5; break;
                default:
                        kernel_panic("PBA BIR invalid.", __FILE__, __LINE__);
        }

        pba_addr = addr_ensure_higher(pba_addr + (msix_cap->mpba & ~0x7));

        printf(KMSG_LOGLEVEL_INFO,
               "MSI-X: Table at %a (size=%d), PBA at %a\n",
               table, table_size, pba_addr);

        table[0].vec_control = 0;
        table[0].lower_addr = 0xfee << 20; /* fixed value for interrupt messages */
        table[0].msg_data = 0x6; /* vector */

        msix_cap->mxc |= 1 << 15; /* enable MSI-X */
}

/* yes, lots of code is duplicated from the create_admin_queue() procedure.
 * too bad! */
void create_io_queue(void)
{
        /* allocate memory and initialize driver structures */
        const size_t sq_page_count = sizeof(struct sq_entry) * 256 / 0x1000;
        const size_t cq_page_count = sizeof(struct cq_entry) * 256 / 0x1000;
        io_queue.sq = (struct sq_entry*)
                addr_ensure_higher((uint64_t)pmm_request_pages(sq_page_count));
        io_queue.cq = (struct cq_entry*)
                addr_ensure_higher((uint64_t)pmm_request_pages(cq_page_count));
        io_queue.sq_tail = 0;
        io_queue.cq_head = 0;

        const uint32_t doorbell_stride = 1 << (2 + (regs->cap >> 32 & 0xf));
        io_queue.sq_tail_dbl = (uint32_t*)
                ((uint64_t)regs + 0x1000 + (2 * 1 + 0) * doorbell_stride);
        io_queue.cq_head_dbl = (uint32_t*)
                ((uint64_t)regs + 0x1000 + (2 * 1 + 1) * doorbell_stride);

        /* create completion queue on NVM device */
        struct sq_entry command = {
                .cdw0 = 0x5,
                .prp1 = addr_ensure_lower((uint64_t)io_queue.cq) >> 0,
                .prp2 = addr_ensure_lower((uint64_t)io_queue.cq) >> 32,
                .cdw10 = 1 << 0 |   /* queue identifier */
                        255 << 16,  /* queue size - 1 */
                .cdw11 = 1 << 0 |   /* physically contiguous */
                        1 << 1 |    /* interrupts enabled */
                        0 << 16     /* MSI-X interrupt vector */
        };

        printf(KMSG_LOGLEVEL_INFO, "Creating I/O completion queue...\n");
        send_cmd_sync(&admin_queue, &command, NULL);

        /* create submission queue on NVM device */
        command.cdw0 = 0x1;
        command.prp1 = addr_ensure_lower((uint64_t)io_queue.sq) >> 0;
        command.prp2 = addr_ensure_lower((uint64_t)io_queue.sq) >> 32;
        command.cdw10 = 1 << 0 |    /* queue identifier */
                255 << 16;          /* queue size - 1 */
        command.cdw11 = 1 << 0 |    /* physically contiguous */
                1 << 16;            /* completion queue identifier */

        printf(KMSG_LOGLEVEL_INFO, "Creating I/O submission queue...\n");
        send_cmd_sync(&admin_queue, &command, NULL);

        printf(KMSG_LOGLEVEL_INFO, "Created I/O queue with sq/cq at %a/%a\n",
               io_queue.sq, io_queue.cq);
}

void namespace_identify(void)
{
        nsid = (uint32_t*)addr_ensure_higher((uint64_t)pmm_request_pages(1));

        struct sq_entry command = {
                .cdw0 = 0x06,
                .prp1 = addr_ensure_lower((uint64_t)nsid),
                .cdw10 = 2
        };
        send_cmd_sync(&admin_queue, &command, NULL);

        nsid_count = 0;
        while (nsid[nsid_count]) {
                nsid_count++;
        }

        printf(KMSG_LOGLEVEL_INFO, "%d namespace(s) found: ", nsid_count);
        for (size_t i = 0; i < nsid_count; i++) {
                printf(KMSG_LOGLEVEL_NONE, "%d ", nsid[i]);
        }
        printf(KMSG_LOGLEVEL_NONE, "\n");

        namespace_id = (struct namespace_id*)
                addr_ensure_higher((uint64_t)pmm_request_pages(1));
        command.prp1    = addr_ensure_lower((uint64_t)namespace_id);
        command.cdw10   = 0;
        command.nsid    = nsid[0];
        send_cmd_sync(&admin_queue, &command, NULL);

        const struct lba_format *format =
                &namespace_id->lba_formats[namespace_id->flbas & 0x3];

        printf(KMSG_LOGLEVEL_INFO,
               "Namespace size/capacity/utilization: %d/%d/%d blocks, "
               "LBA data/metadata size=%d/%d bytes, relative perf=%d\n",
               namespace_id->nsze, namespace_id->ncap, namespace_id->nuse,
               1 << format->lbads, format->ms, format->rp);
}

void ctrl_identify(void)
{
        ctrl_id = (struct ctrl_id*)
                addr_ensure_higher((uint64_t)pmm_request_pages(1));

        struct sq_entry command = {
                .cdw0 = 0x06,
                .prp1 = addr_ensure_lower((uint64_t)ctrl_id),
                .cdw10 = 1
        };

        send_cmd_sync(&admin_queue, &command, NULL);

        trim_and_terminate(ctrl_id->sn, 20);
        trim_and_terminate(ctrl_id->mn, 40);
        trim_and_terminate(ctrl_id->fr, 8);

        printf(KMSG_LOGLEVEL_INFO,
               "Model/Serial: %s/%s, Firmware Revision %s\n",
               ctrl_id->sn, ctrl_id->mn, ctrl_id->fr);

        /* TNVMCAP and UNVMCAP attribute supported */
        if (ctrl_id->oacs >> 3 & 1) {
                printf(KMSG_LOGLEVEL_INFO,
                       "Total/Unused NVM capacity: approx. %d/%dMB\n",
                       ctrl_id->tnvmcap[1] >> 20, ctrl_id->unvmcap[1] >> 20);
        }
}

void send_cmd_sync(struct queue *queue,
                   const struct sq_entry *command,
                   struct cq_entry *result)
{
        /* submit */
        queue->sq[queue->sq_tail] = *command;
        *(queue->sq_tail_dbl) = (queue->sq_tail + 1) % 256;
        queue->sq_tail = (queue->sq_tail + 1) % 256;

        /* wait for completion */
        while ((queue->cq[queue->cq_head].status_field & 0x1) == 0) {
                asm volatile("hlt");
        }

        if ((queue->cq[queue->cq_head].status_field >> 1) != 0) {
                printf(KMSG_LOGLEVEL_CRIT, "NVMe command error: status=%x\n",
                       queue->cq[queue->cq_head].status_field);
        }

        /* get result */
        queue->cq[queue->cq_head].status_field &= ~(0x1);
        if (result)
                *result = queue->cq[queue->cq_head];

        *(queue->cq_head_dbl) = (queue->cq_head + 1) % 256;
        queue->cq_head = (queue->cq_head + 1) % 256;
}
