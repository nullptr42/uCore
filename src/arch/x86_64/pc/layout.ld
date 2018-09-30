ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
    . = 0xFFFFFFFF80200000;

    kernel_start = .;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000) {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xFFFFFFFF80000000) {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xFFFFFFFF80000000) {
        *(COMMON)
        *(.bss)
    }

    /* Page Align */
    .= ALIGN(4096);
    kernel_end = .;
}
