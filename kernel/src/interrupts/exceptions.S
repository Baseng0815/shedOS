.extern exception_handle

.macro push_all
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
.endm

.macro pop_all
    popq %rax
    popq %rbx
    popq %rcx
    popq %rdx
    popq %rsi
    popq %rdi
    popq %rbp
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15
.endm

.macro isr_nocode int_no name
    .global \name
    \name:
    pushq $0
    pushq $\int_no
    push_all
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    iretq
.endm

.macro isr_code int_no name
    .global \name
    \name:
    pushq $\int_no
    push_all
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    iretq
.endm

isr_nocode 0x0 de_handle
isr_nocode 0x1 db_handle
isr_nocode 0x3 bp_handle
isr_nocode 0x4 of_handle
isr_nocode 0x5 br_handle
isr_nocode 0x6 ud_handle
isr_nocode 0x7 nm_handle
isr_code 0x8 df_handle
isr_code 0xa ts_handle
isr_code 0xb np_handle
isr_code 0xc ss_handle
isr_code 0xd gp_handle
isr_code 0xe pf_handle
isr_nocode 0x10 mf_handle
isr_code 0x11 ac_handle
isr_nocode 0x12 mc_handle
isr_nocode 0x13 xm_handle
isr_nocode 0x14 ve_handle
isr_code 0x1e sx_handle
