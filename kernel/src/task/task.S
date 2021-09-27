.global switch_to_task
switch_to_task:
    pushq %rbx
    pushq %rsp
    pushq %rbp

    movq %rsp, %rax
    movq 128(%rdi), %rsp
    pushq %rax
    call 136(%rdi)
    popq %rsp

    popq %rbp
    popq %rsp
    popq %rbx
    retq
