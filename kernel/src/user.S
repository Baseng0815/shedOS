.global _user_jump
.extern user_jump
_user_jump:
    cli
    movw $0x23, %ax # user data
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs # ss is handled by iret

    # the IRET instruction pops the return instruction pointer, return segment
    # selector and EFLAGS from the stack and also the stack pointer and SS if
    # the return is to another privilege level (which is exactly what we need)
    movq %rsp, %rax
    pushq $0x23 # user data
    pushq %rax
    pushfq
    # popq %rax
    # orq $0x200, %rax # IF enable
    # pushq %rax
    pushq $0x1b # user code
    pushq $user_jump
    iretq
