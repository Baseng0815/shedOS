.global switch_to_task
switch_to_task:
    # ring 3 data segments
    movw $0x23, %ax # user data
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs # ss is handled by iret

    # virtual address space
    movq 152(%rdi), %rax
    movq $0xffff800000000000, %rbx
    subq %rbx, %rax
    movq %rax, %cr3

    # set up stack expected by iretq
    pushq $0x23 # SS
    movq 120(%rdi), %rax
    pushq %rax # RSP
    movq 128(%rdi), %rax
    orq $0x200, %rax # IF enable
    pushq %rax # RFLAGS
    pushq $0x1b # CS (user code)
    movq 136(%rdi), %rax
    pushq %rax # RIP

    # GPRs
    movq 0(%rdi), %r15
    movq 8(%rdi), %r14
    movq 16(%rdi), %r13
    movq 24(%rdi), %r12
    movq 32(%rdi), %r11
    movq 40(%rdi), %r10
    movq 48(%rdi), %r9
    movq 56(%rdi), %r8
    movq 64(%rdi), %rbp
    movq 80(%rdi), %rsi
    movq 88(%rdi), %rdx
    movq 96(%rdi), %rcx
    movq 104(%rdi), %rbx
    movq 112(%rdi), %rax
    movq 72(%rdi), %rdi

    iretq
