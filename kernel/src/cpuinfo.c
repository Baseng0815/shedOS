#include "cpuinfo.h"

#include <stdint.h>
#include <stddef.h>

#define __cpuid(eax) \
        asm volatile("movl %[ieaxv], %%eax; cpuid;" \
                     : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
                     : [ieaxv] "r" (eax)); \

#define __cpuid_leaf(eax,ecx) \
        asm volatile("movl %[ieaxv], %%eax;" \
                     "movl %[iecxv], %%ecx; cpuid;" \
                     : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
                     : [ieaxv] "r" (eax), [iecxv] "r" (ecx)); \

bool cpuinfo_query(struct cpuinfo *info)
{
        uint64_t cpuid_present;
        /* ID bit 0x200000 of EFLAGS is only modifiable if CPUID is supported */
        asm volatile("pushfq;"
                     "pushfq;"
                     "xorq $0x00200000, (%%rsp);"
                     "popfq;"
                     "pushfq;"
                     "popq %%rdx;"
                     "xorq (%%rsp), %%rdx;"
                     "popfq;"
                     "andq $0x00200000, %%rdx;"
                     "movq %%rdx, %0;"
                     : "=m" (cpuid_present));

        if (cpuid_present == 0) {
                return false;
        }

        uint32_t a, b, c, d;
        /* brand string */
        for (uint32_t i = 0; i < 3; i++) {
                __cpuid(0x80000002 + i);
                *((uint32_t*)(info->brand_string + 0  + i * 16)) = a;
                *((uint32_t*)(info->brand_string + 4  + i * 16)) = b;
                *((uint32_t*)(info->brand_string + 8  + i * 16)) = c;
                *((uint32_t*)(info->brand_string + 12 + i * 16)) = d;
        }

        /* cut string off once there are two spaces */
        for (size_t i = 0; i < 47; i++) {
                if (info->brand_string[i] == ' ' &&
                    info->brand_string[i + 1] == ' ') {
                        info->brand_string[i] = '\0';
                }
        }

        /* vendor string */
        info->vendor_string[12] = '\0';
        __cpuid(0x0);
        *((uint32_t*)(info->vendor_string + 0)) = b;
        *((uint32_t*)(info->vendor_string + 4)) = d;
        *((uint32_t*)(info->vendor_string + 8)) = c;

        /* version information */
        __cpuid(0x1);
        info->stepping          = (a & 0x0000000f);
        info->model             = (a & 0x000000f0) >> 4;
        info->family            = (a & 0x00000f00) >> 8;
        info->processor_type    = (a & 0x00003000) >> 12;
        short emodel_id         = (a & 0x000f0000) >> 16;
        short efamily_id        = (a & 0x0ff00000) >> 20;

        if (info->family == 6 || info->family == 15) {
                info->model += emodel_id << 4;
        }

        if (info->family == 15) {
                info->family += efamily_id;
        }

        return true;
}
