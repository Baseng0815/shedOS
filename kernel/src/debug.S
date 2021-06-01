.global register_dump_make
register_dump_make:
    movq %rax, 0(%rdi)
    movq %rbx, 8(%rdi)
    movq %rcx, 16(%rdi)
    movq %rdx, 24(%rdi)
    movq %rsi, 32(%rdi)
    movq %rdi, 40(%rdi)
    movq %rbp, 48(%rdi)
    movq %rsp, 56(%rdi)
    movq %r8, 64(%rdi)
    movq %r9, 72(%rdi)
    movq %r10, 80(%rdi)
    movq %r11, 88(%rdi)
    movq %r12, 96(%rdi)
    movq %r13, 104(%rdi)
    movq %r14, 112(%rdi)
    movq %r15, 120(%rdi)
    /* TODO walk stack to find rip */
    movq $0x0, 128(%rdi)
    movw %es, 136(%rdi)
    movw %cs, 138(%rdi)
    movw %ss, 140(%rdi)
    movw %ds, 142(%rdi)
    movw %fs, 144(%rdi)
    movw %gs, 146(%rdi)
    sidt 148(%rdi)
    sgdt 158(%rdi)
    movq %cr0, %rax
    movq %rax, 168(%rdi)
    movq %cr2, %rax
    movq %rax, 176(%rdi)
    movq %cr3, %rax
    movq %rax, 184(%rdi)
    movq %cr4, %rax
    movq %rax, 192(%rdi)
    movq %dr0, %rax
    movq %rax, 200(%rdi)
    movq %dr1, %rax
    movq %rax, 208(%rdi)
    movq %dr2, %rax
    movq %rax, 216(%rdi)
    movq %dr3, %rax
    movq %rax, 224(%rdi)
    movq %dr6, %rax
    movq %rax, 232(%rdi)
    movq %dr7, %rax
    movq %rax, 240(%rdi)
    ret
