.global switch_to_task
switch_to_task:
    # set up stack expected by iretq
    pushq $0x10 # SS
    movq 120(%rdi), %rax
    pushq %rax # RSP
    movq 128(%rdi), %rax
    pushq %rax # RFLAGS
    pushq $0x08 # CS
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
