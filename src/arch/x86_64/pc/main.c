#include <stdint.h>

#include "gdt.h"

void idt_init();

void kernel_main() {
    gdt_init();
    idt_init();

    uint16_t* vram = (void*)(0xB8000);
    char* str = "µCore Microkernel 0.1";
    
    char c;
    while ((c = *str++)) {
        *vram++ = c | 0x0700;
    }

    asm("sti");
    for(;;) asm("hlt");
}
