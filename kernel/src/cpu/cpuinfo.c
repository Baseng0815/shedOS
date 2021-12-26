#include "cpuinfo.h"

#include <stdint.h>
#include <stddef.h>

#define __cpuid(eax) \
        asm volatile("movl %[ieaxv], %%eax; cpuid;" \
                     : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
                     : [ieaxv] "g" (eax)); \

#define __cpuid_leaf(eax,ecx) \
        asm volatile("movl %[ieaxv], %%eax;" \
                     "movl %[iecxv], %%ecx; cpuid;" \
                     : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
                     : [ieaxv] "g" (eax), [iecxv] "g" (ecx)); \

struct cpuinfo cpuinfo;

void cpuinfo_initialize()
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
                     : "=g" (cpuid_present));

        if (cpuid_present == 0) {
                cpuinfo.supported = false;
                return;
        }

        uint32_t a, b, c, d;
        /* brand string */
        for (uint32_t i = 0; i < 3; i++) {
                __cpuid(0x80000002 + i);
                *((uint32_t*)(cpuinfo.brand_string + 0  + i * 16)) = a;
                *((uint32_t*)(cpuinfo.brand_string + 4  + i * 16)) = b;
                *((uint32_t*)(cpuinfo.brand_string + 8  + i * 16)) = c;
                *((uint32_t*)(cpuinfo.brand_string + 12 + i * 16)) = d;
        }

        /* cut string off once there are two spaces */
        for (size_t i = 0; i < 47; i++) {
                if (cpuinfo.brand_string[i] == ' ' &&
                    cpuinfo.brand_string[i + 1] == ' ') {
                        cpuinfo.brand_string[i] = '\0';
                }
        }

        /* vendor string */
        cpuinfo.vendor_string[12] = '\0';
        __cpuid(0x0);
        *((uint32_t*)(cpuinfo.vendor_string + 0)) = b;
        *((uint32_t*)(cpuinfo.vendor_string + 4)) = d;
        *((uint32_t*)(cpuinfo.vendor_string + 8)) = c;

        /* version information and feature bits */
        __cpuid(0x1);
        cpuinfo.stepping        = (a >> 0)  & 0xf;
        cpuinfo.model           = (a >> 4)  & 0xf;
        cpuinfo.family          = (a >> 8)  & 0xf;
        cpuinfo.processor_type  = (a >> 12) & 0x3;
        short emodel_id         = (a >> 16) & 0xf;
        short efamily_id        = (a >> 20) & 0xff;

        if (cpuinfo.family == 6 || cpuinfo.family == 15) {
                cpuinfo.model += emodel_id << 4;
        }

        if (cpuinfo.family == 15) {
                cpuinfo.family += efamily_id;
        }

        cpuinfo.featureset = d | ((uint64_t)c << 32);

        /* clock speeds and frequencies */
        __cpuid(0x16);
        cpuinfo.base_frequency  = a & 0xffff;
        cpuinfo.max_frequency   = b & 0xffff;
        cpuinfo.bus_frequency   = c & 0xffff;

        __cpuid(0x15);
        cpuinfo.tsc_ratio_denom     = a;
        cpuinfo.tsc_ratio_numer     = b;
        cpuinfo.core_crystal_freq   = c;
}

const char *cpu_featureset[64] = {
        /* first 32 bit */
        "fpu", "vme", "de", "pse",
        "tsc", "msr", "pae", "mce",
        "cx8", "apic",  "RESERVED", "sep",
        "mtrr", "pge", "mca", "cmov",
        "pat", "pse-36", "psn", "clfsh",
        "RESERVED", "ds", "acpi", "mmx",
        "fxsr", "sse", "sse2", "ss",
        "htt", "tm", "ia64", "pbe",
        /* last 32 bit */
        "sse3", "pclmulqdq", "dtes64", "monitor",
        "ds-cpl", "vmx", "smx", "est",
        "tm2", "ssse3", "cnxt-id", "sdbg",
        "fma", "xc16", "xtpr", "pdcm",
        "RESERVED", "pcid", "dca", "sse4.1",
        "sse4.2", "x2apic", "movbe", "popcnt",
        "tsc-deadline", "aes", "xsave", "osxsave",
        "avx", "f16c", "rdrnd", "hypervisor"
};
