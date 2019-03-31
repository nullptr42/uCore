/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stdint.h>
#include <lib/terminal/emulator.hpp>

struct Console : public terminal::Display {
  static const int kVgaPortRegister = 0x3D4;
  static const int kVgaPortData = 0x3D5;
  static const int kVgaCursorReg = 14;

  static const uint8_t kColorMap[16];

  Console() : Display(80, 25) {}

  auto ToCode(terminal::Char const &symbol) -> uint16_t;
  void SetCursor(terminal::Point const &p) final override;
  void Present(terminal::Point const &p1, terminal::Point const &p2,
               const terminal::Char *frame) final override;
};