
#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/gdt.hpp>

using namespace arch::x86_64;

#define IDT_NUM_ENT (256)
#define IDT_LIMIT (IDT_NUM_ENT * sizeof(struct idt_entry) - 1)

enum idt_flags {
    // Gate Type
    IDT_GATE_TASK = 5,
    IDT_GATE_INTR = 6,
    IDT_GATE_TRAP = 7,

    // 64-bit or 32-bit entry. Always 1 in long mode.
    IDT_LM = 1<<3,

    // Minimum Descriptor Privilege Level (DPL)
    IDT_RING_0 = 0<<5,
    IDT_RING_1 = 1<<5,
    IDT_RING_2 = 2<<5,
    IDT_RING_3 = 3<<5,

    IDT_ACTIVE = 1<<7
};

struct idt_pointer {
    uint16_t limit;
    void* pointer;
} __attribute__((packed));

struct idt_entry {
    uint16_t offset_lo;
    uint16_t segment;
    uint8_t  ist; /* interrupt stack table */
    uint8_t  flags;
    uint16_t offset_mi;
    uint32_t offset_hi;
    uint32_t padding;
} __attribute__((packed));

static struct idt_entry idt[IDT_NUM_ENT];

static struct idt_pointer idt_ptr = {
    .limit   = IDT_LIMIT,
    .pointer = &idt[0]
};

// Exception stubs
extern "C" void isr_routine_0x00();
extern "C" void isr_routine_0x01();
extern "C" void isr_routine_0x02();
extern "C" void isr_routine_0x03();
extern "C" void isr_routine_0x04();
extern "C" void isr_routine_0x05();
extern "C" void isr_routine_0x06();
extern "C" void isr_routine_0x07();
extern "C" void isr_routine_0x08();
extern "C" void isr_routine_0x09();
extern "C" void isr_routine_0x0A();
extern "C" void isr_routine_0x0B();
extern "C" void isr_routine_0x0C();
extern "C" void isr_routine_0x0D();
extern "C" void isr_routine_0x0E();
extern "C" void isr_routine_0x0F();
extern "C" void isr_routine_0x10();
extern "C" void isr_routine_0x11();
extern "C" void isr_routine_0x12();
extern "C" void isr_routine_0x13();
extern "C" void isr_routine_0x14();
extern "C" void isr_routine_0x15();
extern "C" void isr_routine_0x16();
extern "C" void isr_routine_0x17();
extern "C" void isr_routine_0x18();
extern "C" void isr_routine_0x19();
extern "C" void isr_routine_0x1A();
extern "C" void isr_routine_0x1B();
extern "C" void isr_routine_0x1C();
extern "C" void isr_routine_0x1D();
extern "C" void isr_routine_0x1E();
extern "C" void isr_routine_0x1F();

// IRQ stubs
extern "C" void isr_routine_0x20();
extern "C" void isr_routine_0x21();
extern "C" void isr_routine_0x22();
extern "C" void isr_routine_0x23();
extern "C" void isr_routine_0x24();
extern "C" void isr_routine_0x25();
extern "C" void isr_routine_0x26();
extern "C" void isr_routine_0x27();
extern "C" void isr_routine_0x28();
extern "C" void isr_routine_0x29();
extern "C" void isr_routine_0x2A();
extern "C" void isr_routine_0x2B();
extern "C" void isr_routine_0x2C();
extern "C" void isr_routine_0x2D();
extern "C" void isr_routine_0x2E();
extern "C" void isr_routine_0x2F();

// Syscall stub
extern "C" void isr_routine_0x30();

extern "C" void idt_reload(struct idt_pointer* pointer);

typedef void (*IrqHandler)();

static void idt_set_entry(struct idt_entry* entry,
                                 IrqHandler handler,
                                 int codesegment,
                                 int flags) {
    uint64_t offset = (uint64_t)handler;

    entry->offset_lo =  offset  & 0xFFFF;
    entry->offset_mi = (offset >> 16) & 0xFFFF;
    entry->offset_hi =  offset >> 32;
    entry->ist       = 0;
    entry->flags     = flags;
    entry->segment   = codesegment;
    entry->padding   = 0;
}

void idt_init() {
    for (int i = 0; i < IDT_NUM_ENT; i++)
        idt_set_entry(&idt[i], NULL, 0, 0);

    idt_set_entry(&idt[0x00], isr_routine_0x00, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x01], isr_routine_0x01, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x02], isr_routine_0x02, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x03], isr_routine_0x03, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x04], isr_routine_0x04, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x05], isr_routine_0x05, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x06], isr_routine_0x06, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x07], isr_routine_0x07, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x08], isr_routine_0x08, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x09], isr_routine_0x09, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0A], isr_routine_0x0A, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0B], isr_routine_0x0B, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0C], isr_routine_0x0C, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0D], isr_routine_0x0D, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0E], isr_routine_0x0E, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x0F], isr_routine_0x0F, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x10], isr_routine_0x10, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x11], isr_routine_0x11, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x12], isr_routine_0x12, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x13], isr_routine_0x13, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x14], isr_routine_0x14, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x15], isr_routine_0x15, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x16], isr_routine_0x16, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x17], isr_routine_0x17, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x18], isr_routine_0x18, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x19], isr_routine_0x19, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1A], isr_routine_0x1A, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1B], isr_routine_0x1B, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1C], isr_routine_0x1C, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1D], isr_routine_0x1D, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1E], isr_routine_0x1E, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x1F], isr_routine_0x1F, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x20], isr_routine_0x20, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x21], isr_routine_0x21, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x22], isr_routine_0x22, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x23], isr_routine_0x23, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x24], isr_routine_0x24, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x25], isr_routine_0x25, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x26], isr_routine_0x26, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x27], isr_routine_0x27, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x28], isr_routine_0x28, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x29], isr_routine_0x29, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2A], isr_routine_0x2A, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2B], isr_routine_0x2B, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2C], isr_routine_0x2C, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2D], isr_routine_0x2D, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2E], isr_routine_0x2E, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);
    idt_set_entry(&idt[0x2F], isr_routine_0x2F, gdt::kKernelCodeSeg, IDT_ACTIVE|IDT_RING_0|IDT_GATE_INTR|IDT_LM);

    idt_reload(&idt_ptr);
}
