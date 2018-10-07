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

    snprintf(buffer, 255, "this is a test %+8.4d %i\n", 123, -12321);
    kprint(buffer);
    
    asm("sti");
    for(;;) asm("hlt");
}
