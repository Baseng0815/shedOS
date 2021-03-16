#ifndef _CPUINFO_H
#define _CPUINFO_H

#include <stdbool.h>
#include <stdint.h>

struct cpuinfo {
        char        brand_string[48];   /* Intel Core i5-8400       */
        char        vendor_string[13];  /* GenuineIntel             */
        short       stepping;           /* 0                        */
        short       model;              /* 113                      */
        short       family;             /* 23                       */
        short       processor_type;     /* should be 0              */
        uint64_t    featureset;         /* some bits are reserved!  */
};

/* returns false if CPUID is unsupported */
bool cpuinfo_query(struct cpuinfo*);

extern const char *cpu_featureset[64];

#endif
