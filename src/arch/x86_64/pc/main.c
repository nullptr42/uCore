#include <stdint.h>

#include "gdt.h"
#include "console.h"

void idt_init();

void kernel_main() {
    gdt_init();
    idt_init();
    print_init();

    kprint("uCore microkernel 0.1\n\n");
    
    asm("sti");
    for(;;) asm("hlt");
}
