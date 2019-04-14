/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <lib/rxx/allocator.hpp>
#include <stdint.h>

/**
 * Initial bump allocator used to bootstrap memory management.
 */
template <size_t capacity> struct BumpAllocator : public rxx::Allocator {
  uint8_t data[capacity];
  uint8_t *position = &data[0];

  void *Alloc(size_t size) final {
    if ((position + size) > &data[capacity]) {
      return nullptr;
    } else {
      void *ptr = position;
      position += size;
      return ptr;
    }
  }

  void Free([[maybe_unused]] void *ptr) final {}
};

BumpAllocator<32768> g_bump_alloc;

rxx::Allocator *g_allocator = &g_bump_alloc;

static void *wrap_alloc(size_t size) {
  void *data = g_allocator->Alloc(sizeof(rxx::Allocator *) + size);
  ((rxx::Allocator **)data)[0] = g_allocator;
  return data + sizeof(rxx::Allocator *);
}

static void wrap_free(void *data) {
  data -= sizeof(rxx::Allocator *);
  rxx::Allocator *allocator = ((rxx::Allocator **)data)[0];
  allocator->Free(data);
}

void *operator new(size_t size) { return wrap_alloc(size); }

void *operator new[](size_t size) { return wrap_alloc(size); }

void operator delete(void *ptr) { wrap_free(ptr); }

void operator delete[](void *ptr) { wrap_free(ptr); }
