/* the bootloader will look at this image and start execution at this symbol */
ENTRY(_start)

SECTIONS
{
    /* begin putting sections at 1MiB, conventional place */
    . = 1M;

    /* first put the multiboot headerk then the .text section */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }
    /* read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    /* read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }
    /* read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
