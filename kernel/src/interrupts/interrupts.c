#include "interrupts.h"

#include "apic.h"
#include "timer.h"
#include "hpet.h"

#include "../memory/addrutil.h"
#include "../memory/cow.h"

#include "../libk/printf.h"
#include "../syscall/syscalls.h"

static const char *exception_names[] = {
        "Divide by zero exception (0x00)",
        "Debug exception (0x01)",
        "Invalid exception ISR",
        "Breakpoint exception (0x03)",
        "Overflow exception (0x04)",
        "Bound range exceeded (0x05)",
        "Invalid opcode (0x06)",
        "Device not available (0x07)",
        "Double fault (0x08)",
        "Invalid exception ISR",
        "Invalid TSS (0x0a)",
        "Segment not present (0x0b)",
        "Stack segment fault (0x0c)",
        "General protection fault (0x0d)",
        "Page fault (0x0e)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Alignment check exception (0x11)",
        "Machine check exception (0x12)",
        "SIMD floating point exception (0x13)",
        "Virtualization exception (0x14)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Invalid exception ISR",
        "x86 floating point exception (0x10)",
        "Security exception (0x1e)"
};

/* all exceptions are handled by this procedure */
void exception_handle(struct exception_frame *frame)
{
        if (frame->int_no == EXCEPTION_PF) {
                uint64_t faulting_address;
                asm volatile("movq %%cr2, %0"
                             : "=g" (faulting_address));
                if (ptr_is_user((void*)faulting_address)) {
                        /* page fault comes from user address */
                        if (cow_copy_on_write((void*)faulting_address)) {
                                return;
                        }
                }
        } else if (frame->int_no == EXCEPTION_DB) {
                return;
        }

        printf(KMSG_LOGLEVEL_CRIT,
               "%s: err=%x\n"
               "rax=%x, rbx=%x, rcx=%x, rdx=%x\n"
               "rsi=%x, rdi=%x, rbp=%x, rsp=%x\n"
               "r8=%x, r9=%x, r10=%x, r11=%x\n"
               "r12=%x, r13=%x, r14=%x, r15=%x\n"
               "rip=%x, cs=%x, rflags=%x, ss=%x",
               exception_names[frame->int_no], frame->error_code,
               frame->gprs.rax, frame->gprs.rbx, frame->gprs.rcx,
               frame->gprs.rdx, frame->gprs.rsi, frame->gprs.rdi,
               frame->gprs.rbp, frame->frame.rsp, frame->gprs.r8,
               frame->gprs.r9, frame->gprs.r10, frame->gprs.r11,
               frame->gprs.r12, frame->gprs.r13, frame->gprs.r14,
               frame->gprs.r15, frame->frame.rip, frame->frame.cs,
               frame->frame.rflags, frame->frame.ss);

        for (;;) {
                asm volatile("hlt");
        }
}

/* HPET */
void isr34(struct interrupt_frame *frame)
{
        apic_send_eoi();

        static uint64_t prev_time = 0;

        uint64_t current_time = hpet_read_counter();
        uint64_t dus = current_time - prev_time;
        prev_time = current_time;
        timer_tick(frame, dus);
}

uint64_t isr128(struct interrupt_frame *frame)
{
        uint8_t syscall_id = frame->gprs.rax;
        if (!syscalls[syscall_id]) {
                printf(KMSG_LOGLEVEL_WARN,
                       "Received invalid syscall %d\n",
                       syscall_id);
                return 1;
        }

        return syscalls[syscall_id](frame);
}
