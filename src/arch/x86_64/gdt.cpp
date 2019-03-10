
#include <stdint.h>
#include <arch/x86_64/gdt.hpp>
#include <stdio.hpp>

namespace arch::x86_64::gdt {

enum Flags {
    Coarse = (1<<3),
    Mode32 = (1<<2),
    Mode64 = (1<<1)
};

enum AccessByte {
    Present = 1<<7,
    Ring0   = 0<<5,
    Ring1   = 1<<5,
    Ring2   = 2<<5,
    Ring3   = 3<<5,
    Segment = 1<<4,
    Executable = 1<<3,
    Conforming = 1<<2,
    Writeable  = 1<<1,
    Accessed   = 1<<0,

    AccessRing0 = (AccessByte::Present | AccessByte::Ring0 | AccessByte::Segment | AccessByte::Writeable),
    AccessRing3 = (AccessByte::Present | AccessByte::Ring3 | AccessByte::Segment | AccessByte::Writeable)
};

struct GdtEntry {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t  base_mi;
    uint8_t  access;
    uint8_t  limit_hi : 4;
    uint8_t  flags    : 4;
    uint8_t  base_hi;

    GdtEntry(uint32_t base, uint32_t limit, int flags, int access) {
        base_lo  =  base & 0xFFFF;
        base_mi  = (base >> 16) & 0xFF;
        base_hi  =  base >> 24;
        limit_lo =  limit & 0xFFFF;
        limit_hi = (limit >> 16) & 0xF;
        this->access = access & 0xFF;
        this->flags  = flags  & 0xF;
    }
} __attribute__((packed));

struct GdtPointer {
    uint16_t  limit;
    GdtEntry* pointer;
} __attribute__((packed));

extern "C" void gdt_reload(GdtPointer* pointer,
                           uint8_t codesegment,
                           uint8_t datasegment
                          );

static GdtEntry g_gdt_table[] = {
    { 0, 0, 0, 0 },
    { 0, 0xFFFFF, Flags::Coarse | Flags::Mode64, AccessByte::AccessRing0 | AccessByte::Executable },
    { 0, 0xFFFFF, Flags::Coarse | Flags::Mode32, AccessByte::AccessRing0 },
    { 0, 0xFFFFF, Flags::Coarse | Flags::Mode64, AccessByte::AccessRing3 | AccessByte::Executable },
    { 0, 0xFFFFF, Flags::Coarse | Flags::Mode32, AccessByte::AccessRing3 }
};

static GdtPointer g_gdt_pointer = {
    .limit = sizeof(g_gdt_table)-1,
    .pointer = &g_gdt_table[0]
};

void initialize() {
    /* Tell CPU to load the new GDT */
    gdt_reload(&g_gdt_pointer, kKernelCodeSeg, kKernelDataSeg);
}

}
