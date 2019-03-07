
#pragma once

#include <stddef.h>

namespace lib {

/**
 * Generic interface for memory allocation.
 */
struct Allocator {
    virtual void* Alloc(size_t size) = 0;
    virtual void  Free(void* ptr) = 0;
};

}