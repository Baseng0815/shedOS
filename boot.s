MBALIGN     equ 1 << 0              ; align loaded modules on page boundaries
MEMINFO     equ 1 << 1              ; provide memory map
FLAGS       equ MBALIGN |MEMINFO    ; multiboot flag field
MAGIC       equ 0x1BADB002          ; multiboot magic number
CHECKSUM    equ -(MAGIC + FLAGS)    ; checksum to prove we are multiboot

; declare multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; multiboot does not define value of stack pointer register so we have to
; provide a stack. The stack on x86 must be 16-byte aligned according to SVABI
; the stack grows downwards on x86
section .bss
align 16
stack_bottom:
    resb 16384 ;16 KiB
stack_top:

; the linker script specifies _start as the kernel entry point and the
; bootloader will jump to this position once the kernel has been loaded
section .text
global _start:function (_start.end - _start)
_start:
    ; bootloader has loaded us into 32-bit protected mode on x86 with disabled
    ; interrupts and disabled paging. The kernel has full control over the CPU.
    mov esp, stack_top

    ; crucial processor state should be initialized here. Processor is not yet
    ; fully initialized: floating-point instr are not yet initialized.
    ; The GDT should be loaded and paging should be enabled here
    extern kernel_main
    call kernel_main

    ; if the system has nothing more to do, put coputer in infinite loop:
    ; (1) disable interrupts with cli
    ; (2) wait for next interrupt to arrive with hlt
    ; (3) jmp to hlt instr
    cli
.hang:
    jmp .hang
.end:
