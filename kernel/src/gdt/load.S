.global gdt_load
.global tss_load

gdt_load:
    lgdt (%rdi)
    movw $0x10, %ax # kernel data
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    popq %rdi
    movq $0x08, %rax # kernel code
    pushq %rax
    pushq %rdi
    retfq

tss_load:
    movw $0x28, %ax # tss
    ltr %ax
    ret
