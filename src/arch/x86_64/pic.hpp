/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stdint.h>

namespace arch::x86_64::pic {

void initialize();
void set_mask(uint16_t irq_mask);
void send_eoi(int irq);

} // namespace arch::x86_64::pic