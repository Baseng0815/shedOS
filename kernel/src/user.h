/* this file is for testing only and will be removed later on */

#ifndef _USER_H
#define _USER_H

#include <stdint.h>

struct tss {
        uint32_t reserved0;
        uint32_t rsp0[2];
        uint32_t rsp1[2];
        uint32_t rsp2[2];
        uint32_t reserved1[2];
        uint32_t ist1[2];
        uint32_t ist2[2];
        uint32_t ist3[2];
        uint32_t ist4[2];
        uint32_t ist5[2];
        uint32_t ist6[2];
        uint32_t ist7[2];
        uint32_t reserved2[2];
} __attribute__((packed));

void user_jump();

#endif
