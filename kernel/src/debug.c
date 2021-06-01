#include "debug.h"

#include "libk/printf.h"

void kernel_panic(const char *message, const char *file, int line)
{
        struct register_dump rd;
        register_dump_make(&rd);

        printf(KMSG_LOGLEVEL_CRIT, "KERNEL PANIC: %s at %s:%d\n",
               message, file, line);

        printf(KMSG_LOGLEVEL_NONE,
               "rax=%a, rbx=%a, rcx=%a, rdx=%a\n"
               "rsi=%a, rdi=%a, rbp=%a, rsp=%a\n"
               "r8 =%a, r9 =%a, r10=%a, r11=%a\n"
               "r12=%a, r13=%a, r14=%a, r15=%a\n"
               "es =%a, cs =%a, ss =%a, ds =%a, fs=%a, gs=%a\n"
               "IDT=%a, lim=%x\n"
               "GDT=%a, lim=%x\n"
               "cr0=%a, cr2=%a, cr3=%a, cr4=%a\n"
               "dr0=%a, dr1=%a, dr2=%a, dr3=%a, dr6=%a, dr7=%a\n",
               rd.rax, rd.rbx, rd.rcx, rd.rcx,
               rd.rsi, rd.rdi, rd.rbp, rd.rsp,
               rd.r8, rd.r9, rd.r10, rd.r11,
               rd.r12, rd.r13, rd.r14, rd.r15,
               rd.es, rd.cs, rd.ss, rd.ds, rd.fs, rd.gs,
               rd.idt_base, rd.idt_limit,
               rd.gdt_base, rd.gdt_limit,
               rd.cr0, rd.cr2, rd.cr3, rd.cr4,
               rd.dr0, rd.dr1, rd.dr2, rd.dr3, rd.dr6, rd.dr7);

        for (;;) {
                asm volatile("hlt");
        }
}
