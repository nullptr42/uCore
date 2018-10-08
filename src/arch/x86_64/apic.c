#include "apic.h"
#include "cpuid.h"

#define CPUID_EDX_APIC (1<<9)

bool apic_is_present() {
    struct cpuid_result cpu;

    _cpuid(CPUID_GETFEATURES, &cpu);
    return cpu.edx & CPUID_EDX_APIC;
}

void apic_init() {
}