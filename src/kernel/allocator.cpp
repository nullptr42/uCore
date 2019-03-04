
#include <stdint.h>
#include <lib/allocator.hpp>

/**
 * Initial bump allocator used to bootstrap memory management.
 */
template <size_t capacity>
struct BumpAllocator : public lib::Allocator {
    uint8_t data[capacity];
    uint8_t* position = &data[0];

    void* Alloc(size_t size) final {
        if ((position + size) > &data[capacity]) {
            return nullptr;
        } else {
            void* ptr = position;
            position += size;
            return ptr;
        }
    }

    void Free([[maybe_unused]] void* ptr) final {
    }
};

BumpAllocator<8192> g_bump_alloc;

lib::Allocator* g_allocator = &g_bump_alloc;

void* operator new(size_t size) {
    void* data = g_allocator->Alloc(sizeof(lib::Allocator*) + size);
    ((lib::Allocator**)data)[0] = g_allocator;
    return data + sizeof(lib::Allocator*);
}

void operator delete(void* data) {
    data -= sizeof(lib::Allocator*);
    lib::Allocator* allocator = ((lib::Allocator**)data)[0];
    allocator->Free(data);
}
