
#pragma once

namespace arch::x86_64::gdt {

constexpr uint16_t descriptor(int ring, uint16_t entry) {
  return (ring & 3) | (entry * 8);
}

constexpr uint16_t kKernelCodeSeg = descriptor(0, 1);
constexpr uint16_t kKernelDataSeg = descriptor(0, 2);
constexpr uint16_t kUserCodeSeg = descriptor(3, 3);
constexpr uint16_t kUserDataSeg = descriptor(3, 4);

void initialize();

} // namespace arch::x86_64::gdt