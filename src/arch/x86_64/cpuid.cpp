#include "cpuid.hpp"

namespace arch::x86_64::cpuid {

extern "C" void cpu_cpuid(Function function, Result *result);

void run(Function function, Result &result) { cpu_cpuid(function, &result); }

static CpuVendor get_vendor_id(Result &result) {
  /* Check for "GenuineIntel" */
  if (result.ebx == 0x756e6547 && result.edx == 0x49656e69 &&
      result.ecx == 0x6c65746e)
    return CpuVendor::Intel;

  /* Check for "AuthenticAMD" */
  if (result.ebx == 0x68747541 && result.edx == 0x69746e65 &&
      result.ecx == 0x444d4163)
    return CpuVendor::AMD;

  return CpuVendor::Unsupported;
}

void read(CPU &cpu) {
  Result result;

  /* Get vendor name and highest supported function. */
  uint32_t *vendor = (uint32_t *)&cpu.vendor_name;
  run(Function::GetVendorString, result);
  vendor[0] = result.ebx;
  vendor[1] = result.edx;
  vendor[2] = result.ecx;
  cpu.vendor_name[12] = '\0';
  cpu.vendor = get_vendor_id(result);
  cpu.highest_func = result.eax;

  /* Get feature information. */
  if (cpu.highest_func < uint32_t(Function::GetFeatures))
    return;
  run(Function::GetFeatures, result);
  cpu.features = ((uint64_t)result.edx << 32) | result.ecx;

  /* Decode processor information from eax. */
  cpu.processor.stepping = (result.eax >> 0) & 0xF;
  cpu.processor.model = (result.eax >> 4) & 0xF;
  cpu.processor.family = (result.eax >> 8) & 0xF;
  cpu.processor.type = (result.eax >> 12) & 0x3;
  cpu.processor.ext_model = (result.eax >> 16) & 0xF;
  cpu.processor.ext_family = (result.eax >> 20) & 0xFF;

  /* Decode miscellaneous information from ebx. */
  cpu.misc.brand_id = (result.ebx >> 0) & 0xFF;
  cpu.misc.clflush_size = (result.ebx >> 8) & 0xFF;
  cpu.misc.logical_cpus = (result.ebx >> 16) & 0xFF;
  cpu.misc.apic_id = (result.ebx >> 24) & 0xFF;
}

} // namespace arch::x86_64::cpuid