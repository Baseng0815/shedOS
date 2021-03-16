#ifndef _CPUINFO_H
#define _CPUINFO_H

#include <stdbool.h>

struct cpuinfo {
        char    brand_string[48];   /* brand string (e.g. Intel Core i7 8700k)*/
        char    vendor_string[13];  /* vendor string (e.g. GenuineIntel) */
        short   stepping;
        short   model;              /* model id */
        short   family;             /* family id */
        short   processor_type;     /* processor type (should be 0) */
};

/* returns false if CPUID is unsupported */
bool cpuinfo_query(struct cpuinfo*);

#endif
