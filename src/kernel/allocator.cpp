
#include <stdint.h>
#include <allocator.hpp>

/**
 * Initial bump allocator used to bootstrap memory management.
 */
template <size_t capacity>
struct BumpAllocator : public cxx::Allocator {
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

BumpAllocator<32768> g_bump_alloc;

cxx::Allocator* g_allocator = &g_bump_alloc;

static void* wrap_alloc(size_t size) {
    void* data = g_allocator->Alloc(sizeof(cxx::Allocator*) + size);
    ((cxx::Allocator**)data)[0] = g_allocator;
    return data + sizeof(cxx::Allocator*);
}

static void wrap_free(void* data) {
    data -= sizeof(cxx::Allocator*);
    cxx::Allocator* allocator = ((cxx::Allocator**)data)[0];
    allocator->Free(data);
}

void* operator new(size_t size) {
    return wrap_alloc(size);
}

void* operator new[](size_t size) {
    return wrap_alloc(size);
}

void operator delete(void* ptr) {
    wrap_free(ptr);
}

void operator delete[](void* ptr) {
    wrap_free(ptr);
}
