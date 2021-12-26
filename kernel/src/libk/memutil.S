.global memcpy_128
memcpy_128:
    movq %rdx, %rax
    xorq %rdx, %rdx
    movq $128, %r12
    divq %r12
    movq %rax, %rcx
copy_loop:
    prefetchnta 128(%rsi)
    prefetchnta 160(%rsi)
    prefetchnta 192(%rsi)
    prefetchnta 224(%rsi)

    movdqa 0(%rsi), %xmm0
    movdqa 16(%rsi), %xmm1
    movdqa 32(%rsi), %xmm2
    movdqa 48(%rsi), %xmm3
    movdqa 64(%rsi), %xmm4
    movdqa 80(%rsi), %xmm5
    movdqa 96(%rsi), %xmm6
    movdqa 112(%rsi), %xmm7

    movntdq %xmm0, 0(%rdi)
    movntdq %xmm1, 16(%rdi)
    movntdq %xmm2, 32(%rdi)
    movntdq %xmm3, 48(%rdi)
    movntdq %xmm4, 64(%rdi)
    movntdq %xmm5, 80(%rdi)
    movntdq %xmm6, 96(%rdi)
    movntdq %xmm7, 112(%rdi)

    add $128, %rsi
    add $128, %rdi
    loop copy_loop
    ret
