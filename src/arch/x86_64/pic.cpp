
#include <stdint.h>
#include <arch/x86_64/io_port.hpp>
#include <arch/x86_64/pic.hpp>

namespace arch::x86_64::pic {

#define IRQ_BASE_M (0x20)
#define IRQ_BASE_S (0x28)

/* IO-ports used by the legacy PIC chip */
enum pic_port {
    PIC_IO_M_CMD  = 0x20,
    PIC_IO_M_DATA = 0x21,
    PIC_IO_S_CMD  = 0xA0,
    PIC_IO_S_DATA = 0xA1
};

enum pic_command {
    PIC_INIT_ICW4 = 0x11,
    PIC_CMD_EOI   = 0x20
};

#define PIC_ICW4_8086 (0x01) /* 8086/88 (MCS-80/85) mode */

void initialize() {
    /* Setup master PIC */
    outb(PIC_IO_M_CMD, PIC_INIT_ICW4);
    io_wait();
    outb(PIC_IO_M_DATA, IRQ_BASE_M);
    io_wait();
    outb(PIC_IO_M_DATA, 4); /* Slave PIC is at IRQ2 */
    io_wait();
    outb(PIC_IO_M_DATA, PIC_ICW4_8086);
    io_wait();

    /* Setup slave PIC */
    outb(PIC_IO_S_CMD, PIC_INIT_ICW4);
    io_wait();
    outb(PIC_IO_S_DATA, IRQ_BASE_S);
    io_wait();
    outb(PIC_IO_S_DATA, 2);
    io_wait();
    outb(PIC_IO_S_DATA, PIC_ICW4_8086);
    io_wait();
}

void set_mask(uint16_t irq_mask) {
    outb(PIC_IO_M_DATA, (uint8_t)(irq_mask&0xFF));
    io_wait();
    outb(PIC_IO_S_DATA, (uint8_t)(irq_mask>>8));
    io_wait();
}

void send_eoi(int irq) {
    if (irq >= 16) {
        return;
    }
    
    if (irq >= 8) {
        outb(PIC_IO_S_CMD, PIC_CMD_EOI);
    } else {
        outb(PIC_IO_M_CMD, PIC_CMD_EOI);
    }
    
    io_wait();
}

}
