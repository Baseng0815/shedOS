.section .text
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
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
.endm

.macro isr_nocode int_no
    .global __isr\int_no
    __isr\int_no:
    pushq $0
    pushq $\int_no
    cld
    push_all
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    sti
    iretq
.endm

.macro isr_code int_no
    .global __isr\int_no
    __isr\int_no:
    pushq $\int_no
    cld
    push_all
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    sti
    iretq
.endm

.macro isr_name int_no
    .quad __isr\int_no
.endm

isr_nocode  0
isr_nocode  1
isr_nocode  2
isr_nocode  3
isr_nocode  4
isr_nocode  5
isr_nocode  6
isr_nocode  7
isr_code    8
isr_nocode  9
isr_code    10
isr_code    11
isr_code    12
isr_code    13
isr_code    14
isr_code    15
isr_nocode  16
isr_code    17
isr_nocode  18
isr_nocode  19
isr_nocode  20
isr_nocode  21
isr_nocode  22
isr_nocode  23
isr_nocode  24
isr_nocode  25
isr_nocode  26
isr_nocode  27
isr_nocode  28
isr_nocode  29
isr_code    30

.section .data
.global __exception_interrupts
__exception_interrupts:
    isr_name    0
    isr_name    1
    isr_name    2
    isr_name    3
    isr_name    4
    isr_name    5
    isr_name    6
    isr_name    7
    isr_name    8
    isr_name    9
    isr_name    10
    isr_name    11
    isr_name    12
    isr_name    13
    isr_name    14
    isr_name    15
    isr_name    16
    isr_name    17
    isr_name    18
    isr_name    19
    isr_name    20
    isr_name    21
    isr_name    22
    isr_name    23
    isr_name    24
    isr_name    25
    isr_name    26
    isr_name    27
    isr_name    28
    isr_name    29
    isr_name    30
