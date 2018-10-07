#include <stdint.h>

#include "gdt.h"
#include "console.h"
#include <lib/lib.h>

void idt_init();

void kernel_main() {
    gdt_init();
    idt_init();
    print_init();

    char buffer[255];

    snprintf(buffer, 255, "hello world: %#10x %#10o %%\n", 0xDEADBEEF, 0xDEADBEEF);
    kprint(buffer);
    
    asm("sti");
    for(;;) asm("hlt");
}
