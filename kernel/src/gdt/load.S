.global gdt_load

gdt_load:
        lgdt (%rdi)
        movw $0x10, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss
        popq %rdi
        movq $0x08, %rax
        pushq %rax
        pushq %rdi
        retfq
