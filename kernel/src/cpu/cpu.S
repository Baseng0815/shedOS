.global sse_enable, io_outb, io_inb, io_wait
sse_enable:
    movq %cr0, %rax
    andw $0xfffb, %ax   # CR0.EM clear
    orw $0x2, %ax       # CR0.MP set
    movq %rax, %cr0
    movq %cr4, %rax
    orw $(3<<9), %ax
    movq %rax, %cr4
    ret

io_outb:
    movw %si, %dx   # port
    movb %dil, %al  # value
    outb %al, %dx
    ret

io_inb:
    movw %si, %dx   # port
    xorq %rax, %rax
    inb %dx, %al

io_wait:
    xorb %al, %al
    outb %al, $0x80
    ret
