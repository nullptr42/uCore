ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
    . = 0xFFFFFFFF80100000;

    kernel_start = .;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000) {
        *(.multiboot)
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

    .ctors ALIGN(4K) : AT(ADDR(.ctors) - 0xFFFFFFFF80000000) {
        __ctors_start = .;
        *(.ctors)
        __ctors_end = .;
    }

    /* Page Align */
    .= ALIGN(4096);
    kernel_end = .;
}
