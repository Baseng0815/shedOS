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
        uint32_t nsid;
        uint32_t reserved0[2];
        uint64_t mptr;
        uint64_t prp1;
        uint64_t prp2;
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
        uint16_t wctempt; /* warning composite temperature threshold */
        uint16_t cctemp; /* critical composite temperature threshold */
        uint16_t mtfa; /* maximum time for firmware activation */
        uint32_t hmpre; /* host memory buffer preferred size */
        uint32_t hmmin; /* host memory buffer minimum size */
        uint64_t tnvmcap[2]; /* total NVM capacity */
        uint64_t unvmcap[2]; /* unallocated NVM capacity */
        uint32_t rpmbs; /* replay protected memory block support */
        /* I will finish this maybe some time later */
} __attribute__((packed));

struct regs *regs;
struct queue admin_queue;
struct ctrl_id *ctrl_id;

void send_cmd_sync(struct queue *queue,
                   struct sq_entry *command,
                   struct cq_entry *result);

void ctrl_identify();

void nvme_initialize_device(struct device_table_endpoint *ep,
                            struct pci_addr pci_addr)
{
        printf(KMSG_LOGLEVEL_INFO, "Initializing NVME device (%x/%x/%x)...\n",
               pci_addr.bus, pci_addr.device, pci_addr.function);

        assert((ep->bar0 & 0x4) > 0,
               "NVMe BAR does not support 64-bit mapping.");

        /* find out size of BAR address space */
        const uint32_t prev_bar = ep->bar0;
        ep->bar0 = 0xffffffff;
        const uint32_t cur_bar = ep->bar0 & 0xfffff000;
        ep->bar0 = prev_bar;

        const size_t bar_size = ~cur_bar + 1;
        regs = (struct regs*)vaddr_ensure_higher(
                (ep->bar0 & 0xfffffff0) +
                (ep->bar1 >> 32 & 0xffffffff));
        printf(KMSG_LOGLEVEL_INFO, "BAR=%a, size=%x bytes\n",
               regs, bar_size);

        /* assert capabilities */
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

        /* configure controller and admin queues */
        regs->cc &= ~(1 << 0);

        /* wait for controller to be disabled */
        while (regs->csts & 1 << 0 != 0) {}

        /* AQA, ASQ, ACQ (256 entries each) */
        const size_t asq_page_count = sizeof(struct sq_entry) * 256 / 0x1000;
        const size_t acq_page_count = sizeof(struct cq_entry) * 256 / 0x1000;
        regs->asq = (uint64_t)pmm_request_pages(asq_page_count);
        regs->acq = (uint64_t)pmm_request_pages(acq_page_count);
        regs->aqa =
                255 << 0 | /* submission queue entries */
                255 << 16; /* completion queue entries */

        /* configure admin queue */
        admin_queue.sq = (struct sq_entry*)vaddr_ensure_higher(regs->asq);
        admin_queue.cq = (struct cq_entry*)vaddr_ensure_higher(regs->acq);
        admin_queue.sq_tail = 0;
        admin_queue.cq_head = 0;

        const uint32_t doorbell_stride = 1 << (2 + (regs->cap >> 32 & 0xf));
        admin_queue.sq_tail_dbl = (uint32_t*)
                ((uintptr_t)regs + 0x1000 + 0 * doorbell_stride);
        admin_queue.cq_head_dbl = (uint32_t*)
                ((uintptr_t)regs + 0x1000 + 1 * doorbell_stride);

        printf(KMSG_LOGLEVEL_INFO,
               "ASQ/ACQ at %x/%x, AQA=%x, doorbell stride=%x\n",
               regs->asq, regs->acq, regs->aqa, doorbell_stride);

        regs->intms = 0xffffffff;

        /* reenable controller */
        regs->cc =
                1 << 0  | /* enable */
                0 << 4  | /* NVM command set */
                0 << 7  | /* 2^(12 + 0) = 4K page size */
                0 << 11 | /* round robin */
                0 << 14 | /* no notification */
                6 << 16 | /* 2^6 = 64 byte sqentry size */
                4 << 20;  /* 2^4 = 16 byte cqentry size */

        while (regs->csts & 1 << 0 == 0) {}
        printf(KMSG_LOGLEVEL_INFO, "Controller configured and reenabled.\n");

        ctrl_identify();
}

void send_cmd_sync(struct queue *queue,
                   struct sq_entry *command,
                   struct cq_entry *result)
{
        /* submit */
        queue->sq[queue->sq_tail] = *command;
        *(queue->sq_tail_dbl) = (queue->sq_tail + 1) % 256;

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
}

void ctrl_identify()
{
        ctrl_id = (struct ctrl_identification*)
                vaddr_ensure_higher(pmm_request_pages(1));

        struct sq_entry command = {
                .cdw0 = 0x06,
                .prp1 = vaddr_ensure_lower((uintptr_t)ctrl_id),
                .cdw10 = 1
        };

        send_cmd_sync(&admin_queue, &command, NULL);

        trim_and_terminate(ctrl_id->sn, 20);
        trim_and_terminate(ctrl_id->mn, 40);
        trim_and_terminate(ctrl_id->fr, 8);

        printf(KMSG_LOGLEVEL_INFO,
               "Model/Serial: %s/%s, Firmware Revision %s\n"
               "OACS=%x, MEC=%x, NVMSR=%x, CNTRLTYPE=%x, CTRATT=%x\n",
               ctrl_id->sn, ctrl_id->mn, ctrl_id->fr,
               ctrl_id->oacs, ctrl_id->mec, ctrl_id->nvmsr, ctrl_id->cntrltype,
               ctrl_id->ctratt);

        /* TNVMCAP and UNVMCAP attribute supported */
        if (ctrl_id->oacs >> 3 & 1) {
                printf(KMSG_LOGLEVEL_INFO,
                       "Total/Unused NVM capacity: approx. %d/%dMB\n",
                       ctrl_id->tnvmcap[1] >> 20, ctrl_id->unvmcap[1] >> 20);
        }
}
