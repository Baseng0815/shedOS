#include "exceptions.h"

#include "../debug.h"

void(*exception_interrupts[])(struct interrupt_frame*) = {
        de_handle, db_handle, NULL, bp_handle,
        of_handle, br_handle, ud_handle, nm_handle,
        df_handle, NULL,  ts_handle, np_handle,
        ss_handle, gp_handle, pf_handle, NULL,
        mf_handle, ac_handle, mc_handle, xm_handle,
        ve_handle
};

__attribute__((interrupt)) void de_handle(struct interrupt_frame *iframe) {
        kernel_panic("Divide by zero exception (0x00)");
}

__attribute__((interrupt)) void db_handle(struct interrupt_frame *iframe) {
        kernel_panic("Debug exception (0x01)");
}

__attribute__((interrupt)) void bp_handle(struct interrupt_frame *iframe) {
        kernel_panic("Breakpoint exception (0x03)");
}

__attribute__((interrupt)) void of_handle(struct interrupt_frame *iframe) {
        kernel_panic("Overflow exception (0x04)");
}

__attribute__((interrupt)) void br_handle(struct interrupt_frame *iframe) {
        kernel_panic("Bound range exceeded (0x05)");
}

__attribute__((interrupt)) void ud_handle(struct interrupt_frame *iframe) {
        kernel_panic("Invalid opcode (0x06)");
}

__attribute__((interrupt)) void nm_handle(struct interrupt_frame *iframe) {
        kernel_panic("Device not available (0x07)");
}

__attribute__((interrupt)) void df_handle(struct interrupt_frame *iframe) {
        kernel_panic("Double fault (0x08)");
}

__attribute__((interrupt)) void ts_handle(struct interrupt_frame *iframe) {
        kernel_panic("Invalid TSS (0x0a)");
}

__attribute__((interrupt)) void np_handle(struct interrupt_frame *iframe) {
        kernel_panic("Segment not present (0x0b)");
}

__attribute__((interrupt)) void ss_handle(struct interrupt_frame *iframe) {
        kernel_panic("Stack segment fault (0x0c)");
}

__attribute__((interrupt)) void gp_handle(struct interrupt_frame *iframe) {
        kernel_panic("General protection fault (0x0d)");
}

__attribute__((interrupt)) void pf_handle(struct interrupt_frame *iframe) {
        kernel_panic("Page fault (0x0e)");
}

__attribute__((interrupt)) void mf_handle(struct interrupt_frame *iframe) {
        kernel_panic("x87 floating point exception (0x10)");
}

__attribute__((interrupt)) void ac_handle(struct interrupt_frame *iframe) {
        kernel_panic("Alignment check exception (0x11)");
}

__attribute__((interrupt)) void mc_handle(struct interrupt_frame *iframe) {
        kernel_panic("Machine check exception (0x12)");
}

__attribute__((interrupt)) void xm_handle(struct interrupt_frame *iframe) {
        kernel_panic("SIMD floating point exception (0x13)");
}

__attribute__((interrupt)) void ve_handle(struct interrupt_frame *iframe) {
        kernel_panic("Virtualization exception (0x14)");
}
