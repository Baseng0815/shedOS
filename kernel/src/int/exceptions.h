#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

struct interrupt_frame;
/* exception handlers */
extern void(*exception_interrupts[])(struct interrupt_frame*);

/* divide by zero */
__attribute__((interrupt)) void de_handle(struct interrupt_frame*);
/* debug */
__attribute__((interrupt)) void db_handle(struct interrupt_frame*);
/* breakpoint */
__attribute__((interrupt)) void bp_handle(struct interrupt_frame*);
/* overflow */
__attribute__((interrupt)) void of_handle(struct interrupt_frame*);
/* bound range exceeded */
__attribute__((interrupt)) void br_handle(struct interrupt_frame*);
/* invalid opcode */
__attribute__((interrupt)) void ud_handle(struct interrupt_frame*);
/* device not available */
__attribute__((interrupt)) void nm_handle(struct interrupt_frame*);
/* double fault */
__attribute__((interrupt)) void df_handle(struct interrupt_frame*);
/* invalid TSS */
__attribute__((interrupt)) void ts_handle(struct interrupt_frame*);
/* segment not present */
__attribute__((interrupt)) void np_handle(struct interrupt_frame*);
/* stack segment fault */
__attribute__((interrupt)) void ss_handle(struct interrupt_frame*);
/* general protection fault */
__attribute__((interrupt)) void gp_handle(struct interrupt_frame*);
/* page fault */
__attribute__((interrupt)) void pf_handle(struct interrupt_frame*);
/* x87 floating point exception */
__attribute__((interrupt)) void mf_handle(struct interrupt_frame*);
/* alignment check */
__attribute__((interrupt)) void ac_handle(struct interrupt_frame*);
/* machine check */
__attribute__((interrupt)) void mc_handle(struct interrupt_frame*);
/* SIMD floating point exception */
__attribute__((interrupt)) void xm_handle(struct interrupt_frame*);
/* virtualization exception */
__attribute__((interrupt)) void ve_handle(struct interrupt_frame*);

#endif
