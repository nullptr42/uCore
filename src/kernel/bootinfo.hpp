/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <list.hpp>
#include <stdint.h>

namespace kernel {

struct BootInfo {
  const char *cmdline;

  /* Represents a loadable service executable. */
  struct Module {
    const char *name;
    uint64_t base;
    uint64_t last;
  };

  /* Represents a memory map entry. */
  struct Memory {
    uint64_t base;
    uint64_t last;
  };

  cxx::List<Module> modules;
  cxx::List<Memory> mmap;

  struct Framebuffer {
    bool present;

    void *address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type; /* hackish */

    /* Color format descriptor */
    struct Format {
      uint8_t r_shift;
      uint8_t g_shift;
      uint8_t b_shift;
      uint8_t r_mask_len;
      uint8_t g_mask_len;
      uint8_t b_mask_len;
    } format;
  } fb;
};

} // namespace kernel
