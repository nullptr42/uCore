/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace multiboot {

const uint32_t kMagicNumber = 0x36D76289;

enum class TagType : uint32_t {
  CommandLine = 1,
  Module = 3,
  Memory = 4,
  MemoryMap = 6,
  Framebuffer = 8
};

struct Tag {
  TagType type;
  uint32_t size;
} __attribute__((packed));

struct Module : Tag {
  uint32_t start;
  uint32_t end;
  char string; /* TODO: this is actually an array */
} __attribute__((packed));

struct Memory : Tag {
  uint32_t lower;
  uint32_t upper;
} __attribute__((packed));

struct MemoryMap : Tag {
  uint32_t entry_size;
  uint32_t entry_version;

  enum class Type : uint32_t {
    Reserved = 0,
    Available = 1,
    AcpiInfo = 3,
    Hibernate = 4,
    Defective = 5
  };

  struct Entry {
    uint64_t base;
    uint64_t length;
    Type type;
    uint32_t reserved;
  } __attribute__((packed));

  struct Iterator {
    using value_type = Entry;
    using difference_type = ptrdiff_t;
    using pointer = Entry *;
    using reference = Entry &;

    Iterator(Entry *entry, uint32_t entry_size)
        : position(entry), entry_size(entry_size) {}

    Iterator operator++(int) {
      return Iterator((Entry *)((void *)position + entry_size), entry_size);
    }
    Iterator &operator++() {
      position = (Entry *)((void *)position + entry_size);
      return *this;
    }

    reference operator*() const { return *position; }
    pointer operator->() const { return position; }

    bool operator==(const Iterator &rhs) const {
      return position == rhs.position;
    }
    bool operator!=(const Iterator &rhs) const {
      return position != rhs.position;
    }

  private:
    Entry *position;
    uint32_t entry_size;
  };

  Iterator begin() {
    return Iterator((Entry *)((void *)this + sizeof(MemoryMap)), entry_size);
  }
  Iterator end() {
    return Iterator((Entry *)((void *)this + this->size), entry_size);
  }
} __attribute__((packed));

struct Framebuffer : Tag {
  uint64_t address;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type;
  uint16_t reserved; /* according to documentation this should
                      * be uint8_t but it only works like this ¯\_(ツ)_/¯ */

  /* TODO: support indexed modes (if they are of any use?). */
  uint8_t r_shift;
  uint8_t r_mask_len;
  uint8_t g_shift;
  uint8_t g_mask_len;
  uint8_t b_shift;
  uint8_t b_mask_len;
} __attribute__((packed));

struct Header {
  uint32_t total_size;
  uint32_t reserved;

  struct Iterator {
    using value_type = Tag;
    using difference_type = ptrdiff_t;
    using pointer = Tag *;
    using reference = Tag &;

    Iterator() : position(nullptr) {}
    Iterator(Tag *position) : position(position) {}

    Iterator operator++(int) { return GetNextTag(); }
    Iterator &operator++() {
      position = GetNextTag();
      return *this;
    }

    reference operator*() const { return *position; }
    pointer operator->() const { return position; }

    bool operator==(const Iterator &rhs) const {
      return position == rhs.position;
    }
    bool operator!=(const Iterator &rhs) const {
      return position != rhs.position;
    }

  private:
    Tag *GetNextTag() {
      auto next = (void *)position;
      next += position->size;
      if (((uintptr_t)next % 8) > 0) {
        next += 8 - ((uintptr_t)next % 8);
      }
      return (Tag *)next;
    }

    Tag *position;
  };

  Iterator begin() { return Iterator((Tag *)((void *)this + sizeof(Header))); }
  Iterator end() { return Iterator((Tag *)((void *)this + this->total_size)); }
} __attribute__((packed));

} // namespace multiboot
