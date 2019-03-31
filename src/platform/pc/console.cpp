/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/x86_64/io_port.hpp>
#include <stdint.h>
#include <terminal/emulator.hpp>

#include "console.hpp"

auto Console::ToCode(lib::terminal::Char const &symbol) -> uint16_t {
  int offset = symbol.brightness * 8;

  return symbol.character | (kColorMap[int(symbol.fg) + offset] << 8) |
         (kColorMap[int(symbol.bg)] << 12);
}

void Console::SetCursor(lib::terminal::Point const &p) {
  if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height) {
    /* TODO: Hide/show cursor. */
    return;
  }

  uint16_t address = p.y * width + p.x;

  /* High byte */
  outb(kVgaPortRegister, kVgaCursorReg + 0);
  outb(kVgaPortData, address >> 8);

  /* Low byte */
  outb(kVgaPortRegister, kVgaCursorReg + 1);
  outb(kVgaPortData, address & 0xFF);
}

void Console::Present(lib::terminal::Point const &p1,
                      lib::terminal::Point const &p2,
                      const lib::terminal::Char *frame) {
  for (int y = p1.y; y <= p2.y; y++) {
    auto dst = &((uint16_t *)0xFFFFFFFF800B8000)[y * width];
    auto src = &frame[y * width];
    for (int x = p1.x; x <= p2.x; x++) {
      dst[x] = ToCode(src[x]);
    }
  }
}

const uint8_t Console::kColorMap[] = {0, 4,  2,  6,  1, 5,  3,  7,
                                      8, 12, 10, 14, 9, 13, 11, 15};
