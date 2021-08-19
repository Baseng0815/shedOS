#include "exceptions.h"

#include <stddef.h>

#include "../libk/printf.h"
#include "../debug.h"

/* divide by zero */
extern __attribute__((interrupt)) void de_handle(struct exception_frame*);
/* debug */
extern __attribute__((interrupt)) void db_handle(struct exception_frame*);
/* breakpoint */
extern __attribute__((interrupt)) void bp_handle(struct exception_frame*);
/* overflow */
extern __attribute__((interrupt)) void of_handle(struct exception_frame*);
/* bound range exceeded */
extern __attribute__((interrupt)) void br_handle(struct exception_frame*);
/* invalid opcode */
extern __attribute__((interrupt)) void ud_handle(struct exception_frame*);
/* device not available */
extern __attribute__((interrupt)) void nm_handle(struct exception_frame*);
/* double fault */
extern __attribute__((interrupt)) void df_handle(struct exception_frame*);
/* invalid TSS */
extern __attribute__((interrupt)) void ts_handle(struct exception_frame*);
/* segment not present */
extern __attribute__((interrupt)) void np_handle(struct exception_frame*);
/* stack segment fault */
extern __attribute__((interrupt)) void ss_handle(struct exception_frame*);
/* general protection fault */
extern __attribute__((interrupt)) void gp_handle(struct exception_frame*);
/* page fault */
extern __attribute__((interrupt)) void pf_handle(struct exception_frame*);
/* x87 floating point exception */
extern __attribute__((interrupt)) void mf_handle(struct exception_frame*);
/* alignment check */
extern __attribute__((interrupt)) void ac_handle(struct exception_frame*);
/* machine check */
extern __attribute__((interrupt)) void mc_handle(struct exception_frame*);
/* SIMD floating point exception */
extern __attribute__((interrupt)) void xm_handle(struct exception_frame*);
/* virtualization exception */
extern __attribute__((interrupt)) void ve_handle(struct exception_frame*);

void(*exception_interrupts[])(struct exception_frame*) = {
        de_handle, db_handle, NULL, bp_handle,
        of_handle, br_handle, ud_handle, nm_handle,
        df_handle, NULL,  ts_handle, np_handle,
        ss_handle, gp_handle, pf_handle, NULL,
        mf_handle, ac_handle, mc_handle, xm_handle,
        ve_handle
};

const char *exception_names[] = {
        "Divide by zero exception (0x00)",
        "Debug exception (0x01)",
        "",
        "Breakpoint exception (0x03)",
        "Overflow exception (0x04)",
        "Bound range exceeded (0x05)",
        "Invalid opcode (0x06)",
        "Device not available (0x07)",
        "Double fault (0x08)",
        "",
        "Invalid TSS (0x0a)",
        "Segment not present (0x0b)",
        "Stack segment fault (0x0c)",
        "General protection fault (0x0d)",
        "Page fault (0x0e)",
        "",
        "x86 floating point exception (0x10)",
        "Alignment check exception (0x11)",
        "Machine check exception (0x12)",
        "SIMD floating point exception (0x13)",
        "Virtualization exception (0x14)",
        "", "", "", "", "", "", "", "", "",
        "Security exception (0x1e)"
};

void exception_handle(struct exception_frame *frame)
{
        printf(KMSG_LOGLEVEL_CRIT, "%s: err=%x\n",
               exception_names[frame->int_no], frame->error_code);

        for (;;) {
                asm volatile("hlt");
        }
}
