/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#pragma once

#include <stddef.h>

namespace rxx {

template <typename T> class List {
  struct ListNode {
    ListNode *prev;
    ListNode *next;
    T value;

    ListNode();
    ListNode(const T &value, ListNode *prev = nullptr, ListNode *next = nullptr)
        : prev(prev), next(next), value(value) {}
  };

  ListNode *head = nullptr;
  ListNode *tail = nullptr;

  struct Iterator {
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    using iterator = Iterator;

    ListNode *position;

    Iterator() : position(nullptr) {}
    Iterator(ListNode *node) : position(node) {}

    iterator operator++(int) { return position->next; }

    iterator &operator++() {
      position = position->next;
      return *this;
    }

    reference operator*() const { return position->value; }

    pointer operator->() const { return &position->value; }

    iterator operator+(difference_type value) const {
      auto node = position;
      for (auto i = 0; i < value; i++) {
        node = node->next;
        if (node == nullptr) {
          break;
        }
      }
      return node;
    }

    bool operator==(const iterator &rhs) const {
      return position == rhs.position;
    }

    bool operator!=(const iterator &rhs) const {
      return position != rhs.position;
    }
  };

public:
  List() {}
  ~List() {}

  void InsertBack(const T &value) {
    if (head == nullptr) {
      head = new ListNode(value);
      tail = head;
    } else {
      auto node = new ListNode(value, tail);
      tail->next = node;
      tail = node;
    }
  }

  void InsertFront(const T &value) {
    if (head == nullptr) {
      head = new ListNode(value);
      tail = head;
    } else {
      head = new ListNode(value, nullptr, head);
    }
  }

  Iterator begin() { return Iterator(head); }

  Iterator end() { return Iterator(nullptr); }
};

} // namespace rxx