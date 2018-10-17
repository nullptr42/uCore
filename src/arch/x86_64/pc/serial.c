#include <stdbool.h>
#include <arch/x86_64/io-port.h>

// Placeholder code, until I have a proper implementation.
// Based on: http://wiki.osdev.org/Serial_ports

#define PORT 0x3F8

void serial_init() {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static inline bool serial_received() {
    return inb(PORT + 5) & 1;
}

char serial_read() {
    while(!serial_received()) {}
    
    return inb(PORT);
}

static inline bool serial_can_send() {
    return inb(PORT + 5) & 0x20;
}

void serial_write(char c) {
    while (!serial_can_send()) {}
    
    outb(PORT, c);
}

void serial_debug(const char* string) {
    while (*string)
        serial_write(*string++);
}