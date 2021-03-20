OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    __KERNELSTART__ = ALIGN(0x1000);
    .text : ALIGN(0x1000)
    {
        *(.text)
    }
    .data : ALIGN(0x1000)
    {
        *(.data)
    }
    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }
    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
    __KERNELEND__ = ALIGN(0x1000);
}
