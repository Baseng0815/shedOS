/* the bootloader will start execution here */
ENTRY(_start)

SECTIONS
{
        /* begin putting sections at 1MiB */
        . = 1M;

        /* first put multiboot header and then .text section */
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
        /* read-write data (uninitialized) */
        .data BLOCK(4K) : ALIGN(4K)
        {
                *(.data)
        }
        /* read-write data (initialized) */
        .bss BLOCK(4K) : ALIGN(4K)
        {
                *(COMMON)
                *(.bss)
        }
}
