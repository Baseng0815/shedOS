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

# push error code and int_no
.macro isr_err_nocode int_no
    .global __isr\int_no
    .align 4
    __isr\int_no:
    pushq $0
    pushq $\int_no
    push_all
    cld
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    iretq
.endm

# push int_no
.macro isr_err_code int_no
    .global __isr\int_no
    .align 4
    __isr\int_no:
    pushq $\int_no
    push_all
    cld
    mov %rsp, %rdi
    call exception_handle
    pop_all
    addq $16, %rsp # pop error and int_no
    iretq
.endm

# only gpr and interrupt_frame
.macro isr_noerr int_no
    .global __isr\int_no
    .align 4
    __isr\int_no:
    push_all
    cld
    mov %rsp, %rdi
    call isr\int_no
    pop_all
    iretq
.endm

.macro isr_name int_no
    .quad __isr\int_no
.endm

# exceptions
isr_err_nocode  0
isr_err_nocode  1
isr_err_nocode  2
isr_err_nocode  3
isr_err_nocode  4
isr_err_nocode  5
isr_err_nocode  6
isr_err_nocode  7
isr_err_code    8
isr_err_nocode  9
isr_err_code    10
isr_err_code    11
isr_err_code    12
isr_err_code    13
isr_err_code    14
isr_err_code    15
isr_err_nocode  16
isr_err_code    17
isr_err_nocode  18
isr_err_nocode  19
isr_err_nocode  20
isr_err_nocode  21
isr_err_nocode  22
isr_err_nocode  23
isr_err_nocode  24
isr_err_nocode  25
isr_err_nocode  26
isr_err_nocode  27
isr_err_nocode  28
isr_err_nocode  29
isr_err_code    30

# timer
isr_noerr       34

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
