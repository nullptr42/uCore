/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <reinix/types.h>

namespace rxx {

template <typename T> class Array {

public:
  Array(size_t count) : buffer(new T[count]), count(count), allocated(true) {}

  Array(T *buffer, size_t count)
      : buffer(buffer), count(count), allocated(false) {}

  ~Array() {
    if (allocated) {
      delete buffer;
    }
  }

  T &operator[](int i) {
    /* TODO: check array boundaries. */
    return buffer[i];
  }

  size_t length() const { return count; }

  T *begin() { return buffer; }

  T *end() { return buffer + count; }

private:
  T *buffer;
  size_t count;
  bool allocated;
};

} // namespace rxx