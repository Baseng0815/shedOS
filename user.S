global _user_jump
extern user_jump
_user_jump:
    movw $0x20, %ax ; user code
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs ; ss is handled by iret

    ; the IRET instruction pops the return instruction pointer, return segment
    ; selector and EFLAGS from the stack and also the stack pointer and SS if
    ; the return is to another privilege level (which is exactly what we need)
