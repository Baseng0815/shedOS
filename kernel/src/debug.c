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
               "es =%a, cs =%a, ss =%a, ds =%a, fs=%a, gs=%a\n",
               rd.rax, rd.rbx, rd.rcx, rd.rcx,
               rd.rsi, rd.rdi, rd.rbp, rd.rsp,
               rd.r8, rd.r9, rd.r10, rd.r11,
               rd.r12, rd.r13, rd.r14, rd.r15,
               rd.es, rd.cs, rd.ss, rd.ds, rd.fs, rd.gs);

        for (;;) {
                asm volatile("hlt");
        }
}
