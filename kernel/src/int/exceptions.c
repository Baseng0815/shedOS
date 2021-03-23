#include "exceptions.h"

#include <stdbool.h>

#include <printk.h>

void(*exception_interrupts[])(struct interrupt_frame*) = {
        de_handle, db_handle, NULL, bp_handle,
        of_handle, br_handle, ud_handle, nm_handle,
        df_handle, NULL,  ts_handle, np_handle,
        ss_handle, gp_handle, pf_handle, NULL,
        mf_handle, ac_handle, mc_handle, xm_handle,
        ve_handle
};

__attribute__((interrupt)) void de_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Divide by zero exception (0x00).\n");
        while (true);
}

__attribute__((interrupt)) void db_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Debug exception (0x01).\n");
        while (true);
}

__attribute__((interrupt)) void bp_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Breakpoint exception (0x03).\n");
        while (true);
}

__attribute__((interrupt)) void of_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Overflow exception (0x04).\n");
        while (true);
}

__attribute__((interrupt)) void br_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Bound range exceeded (0x05).\n");
        while (true);
}

__attribute__((interrupt)) void ud_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Invalid opcode (0x06).\n");
        while (true);
}

__attribute__((interrupt)) void nm_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Device not available (0x07).\n");
        while (true);
}

__attribute__((interrupt)) void df_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Double fault (0x08).\n");
        while (true);
}

__attribute__((interrupt)) void ts_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Invalid TSS (0x0a).\n");
        while (true);
}

__attribute__((interrupt)) void np_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Segment not present (0x0b).\n");
        while (true);
}

__attribute__((interrupt)) void ss_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Stack segment fault (0x0c).\n");
        while (true);
}

__attribute__((interrupt)) void gp_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "General protection fault (0x0d).\n");
        while (true);
}

__attribute__((interrupt)) void pf_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Page fault (0x0e).\n");
        while (true);
}

__attribute__((interrupt)) void mf_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "x87 floating point exception (0x10).\n");
        while (true);
}

__attribute__((interrupt)) void ac_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Alignment check exception (0x11).\n");
        while (true);
}

__attribute__((interrupt)) void mc_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Machine check exception (0x12).\n");
        while (true);
}

__attribute__((interrupt)) void xm_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "SIMD floating point exception (0x13).\n");
        while (true);
}

__attribute__((interrupt)) void ve_handle(struct interrupt_frame *iframe) {
        printk(KMSG_LOGLEVEL_CRIT, "Virtualization exception (0x14).\n");
        while (true);
}
