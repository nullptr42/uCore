/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include "include/multiboot2.hpp"

namespace multiboot {

bool multiboot2_verify(uint32_t magic) {
    return magic == MB2_BOOTLOADER_MAGIC;
}

void multiboot2_find_tags(Header* info,
                          TagType type,
                          Callback handler,
                          void* user_argument
                         ) {
    uint32_t  size  = ((Header*)info)->total_size;
    uintptr_t limit = (uintptr_t)info + size;

    void* ptr = (void*)info + sizeof(Header);

    // Walk all tags contained in the info structure.
    while ((uintptr_t)ptr < limit) {
        struct Tag* tag = (struct Tag*)ptr;

        // If the current tag matches the desired type, call the handler function.
        // If the handler function returned true that means we can stop searching.
        if (tag->type == type) {
            if (handler(tag, user_argument)) {
                return;
            }
        }

        // Update pointer to next tag, must ensure tag address is QWORD-aligned.
        ptr += tag->size;
        if ((uintptr_t)ptr & 7) {
            ptr += 8 - ((uintptr_t)ptr & 7);
        }
    }
}

}