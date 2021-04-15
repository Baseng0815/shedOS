#ifndef _CPUINFO_H
#define _CPUINFO_H

#include <stdint.h>
#include <stdbool.h>

struct cpuinfo {
        bool        supported;
        char        brand_string[48];   /* Intel Core i5-8400       */
        char        vendor_string[13];  /* GenuineIntel             */
        short       stepping;           /* 0                        */
        short       model;              /* 113                      */
        short       family;             /* 23                       */
        short       processor_type;     /* should be 0              */
        uint64_t    featureset;         /* some bits are reserved!  */
        short       base_frequency;     /* in MHz                   */
        short       max_frequency;      /* in MHz                   */
        short       bus_frequency;      /* in MHz                   */
        int         tsc_ratio_denom;
        int         tsc_ratio_numer;
        int         core_crystal_freq;  /* in Hz                    */

        /* non-cpuid properties */
        short       core_count;
};

extern struct cpuinfo cpuinfo;
extern const char *cpu_featureset[64];

void cpuinfo_initialize();

#endif
