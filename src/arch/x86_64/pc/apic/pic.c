/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "pic.h"
#include <arch/x86_64/io-port.h>

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

void pic_set_mask(uint16_t irq_mask) {
    outb(PIC_IO_M_DATA, irq_mask&0xFF);
    io_wait();
    outb(PIC_IO_S_DATA, irq_mask>>8);
    io_wait();
}

void pic_init() {
    /* Setup master PIC */
    outb(PIC_IO_M_CMD, PIC_INIT_ICW4);
    io_wait();
    outb(PIC_IO_M_DATA, IRQ_BASE_M);
    io_wait();
    outb(PIC_IO_M_DATA, 4); /* Slave PIC is at IRQ2 */
    io_wait();

    /* Setup slave PIC */
    outb(PIC_IO_S_CMD, PIC_INIT_ICW4);
    io_wait();
    outb(PIC_IO_S_DATA, IRQ_BASE_S);
    io_wait();
    outb(PIC_IO_S_DATA, 2); /* whatever this is */
    io_wait();

    /* Enable all interrupts */
    pic_set_mask(0);  
}

void pic_send_eoi(int irq) {
    if (irq >= 16)
        return;
    if (irq >= 8)
        outb(PIC_IO_S_CMD, PIC_CMD_EOI);
    else
        outb(PIC_IO_M_CMD, PIC_CMD_EOI);
    io_wait();
}
