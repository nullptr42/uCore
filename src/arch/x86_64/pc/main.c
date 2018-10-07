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

    int num;
    snprintf(buffer, 255, "hel%nlo world: %#10x %#10o %% %2c %20s\n", &num, 0xDEADBEEF, 0xDEADBEEF, 'x', "VeryNice");
    kprint(buffer);
    snprintf(buffer, 255, "num=%d\n", num);
    kprint(buffer);
    
    asm("sti");
    for(;;) asm("hlt");
}
