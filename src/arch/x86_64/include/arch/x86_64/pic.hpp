#pragma once

#include <stdint.h>

namespace arch::x86_64::pic {

void initialize();
void set_mask(uint16_t irq_mask);
void send_eoi(int irq);

}