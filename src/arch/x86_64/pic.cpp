
#include <stdint.h>
#include <arch/x86_64/io_port.hpp>
#include <arch/x86_64/pic.hpp>

namespace arch::x86_64::pic {

const int kIrqBaseMaster = 0x20;
const int kIrqBaseSlave  = 0x28;
const int kIcw4Mode8086  = 0x01;

/* IO-ports used by the legacy PIC chip. */
enum Port {
    MasterCommand = 0x20,
    MasterData = 0x21,
    SlaveCommand = 0xA0,
    SlaveData = 0xA1
};

enum Command {
    Initialize = 0x11,
    EndOfInterrupt = 0x20
};

void initialize() {
    /* Setup master PIC */
    outb(Port::MasterCommand, Command::Initialize);
    io_wait();
    outb(Port::MasterData, kIrqBaseMaster);
    io_wait();
    outb(Port::MasterData, 4); /* Slave PIC is at IRQ2 */
    io_wait();
    outb(Port::MasterData, kIcw4Mode8086);
    io_wait();

    /* Setup slave PIC */
    outb(Port::SlaveCommand, Command::Initialize);
    io_wait();
    outb(Port::SlaveData, kIrqBaseSlave);
    io_wait();
    outb(Port::SlaveData, 2);
    io_wait();
    outb(Port::SlaveData, kIcw4Mode8086);
    io_wait();
}

void set_mask(uint16_t irq_mask) {
    outb(Port::MasterData, (uint8_t)(irq_mask&0xFF));
    io_wait();
    outb(Port::SlaveData, (uint8_t)(irq_mask>>8));
    io_wait();
}

void send_eoi(int irq) {
    if (irq >= 16) return;
    
    if (irq >= 8) {
        outb(Port::SlaveCommand, Command::EndOfInterrupt);
    } else {
        outb(Port::MasterCommand, Command::EndOfInterrupt);
    }
    
    io_wait();
}

}
