#include <stdint.h>

#include "gdt.h"
#include "console.h"
#include <lib/lib.h>

void idt_init();

void kernel_main() {
    gdt_init();
    idt_init();
    print_init();

    int num;

    kprintf("hel%nlo world: %#10x %#10o %% %2c %20s\n", &num, 0xDEADBEEF, 0xDEADBEEF, 'x', "VeryNice");
    kprintf("num=%d\n", num);
    
    asm("sti");
    for(;;) asm("hlt");
}
