
#pragma once

#include <stdint.h>

namespace arch::x86_64::cpuid {

enum class Feature : uint64_t {
  SSE3 = 1 << 0,
  PCLMUL = 1 << 1,
  DTES64 = 1 << 2,
  MONITOR = 1 << 3,
  DS_CPL = 1 << 4,
  VMX = 1 << 5,
  SMX = 1 << 6,
  EST = 1 << 7,
  TM2 = 1 << 8,
  SSSE3 = 1 << 9,
  CID = 1 << 10,
  FMA = 1 << 12,
  CX16 = 1 << 13,
  ETPRD = 1 << 14,
  PDCM = 1 << 15,
  PCIDE = 1 << 17,
  DCA = 1 << 18,
  SSE4_1 = 1 << 19,
  SSE4_2 = 1 << 20,
  x2APIC = 1 << 21,
  MOVBE = 1 << 22,
  POPCNT = 1 << 23,
  AES = 1 << 25,
  XSAVE = 1 << 26,
  OSXSAVE = 1 << 27,
  AVX = 1 << 28,
  FPU = 1ULL << 32,
  VME = 1ULL << 33,
  DE = 1ULL << 34,
  PSE = 1ULL << 35,
  TSC = 1ULL << 36,
  MSR = 1ULL << 37,
  PAE = 1ULL << 38,
  MCE = 1ULL << 39,
  CX8 = 1ULL << 40,
  APIC = 1ULL << 41,
  SEP = 1ULL << 43,
  MTRR = 1ULL << 44,
  PGE = 1ULL << 45,
  MCA = 1ULL << 46,
  CMOV = 1ULL << 47,
  PAT = 1ULL << 48,
  PSE36 = 1ULL << 49,
  PSN = 1ULL << 50,
  CLF = 1ULL << 51,
  DTES = 1ULL << 53,
  ACPI = 1ULL << 54,
  MMX = 1ULL << 55,
  FXSR = 1ULL << 56,
  SSE = 1ULL << 57,
  SSE2 = 1ULL << 58,
  SS = 1ULL << 59,
  HTT = 1ULL << 60,
  TM1 = 1ULL << 61,
  IA64 = 1ULL << 62,
  PBE = 1ULL << 63
};

enum class Function : uint32_t {
  GetVendorString = 0,
  GetFeatures = 1,
  GetHighestExtended = 0x80000000,
  GetProcessorName1 = 0x80000002,
  GetProcessorName2 = 0x80000003,
  GetProcessorName3 = 0x80000004
};

enum class CpuVendor { AMD, Intel, Unsupported };

struct Result {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
};

/* TODO: Pick sane defaults. */
struct CPU {
  uint32_t supported;
  uint32_t extended;

  char name[49];
  char vendor_name[13];
  CpuVendor vendor;

  struct Processor {
    int stepping = 0;
    int model = 0;
    int family = 0;
    int type = 0;
    int ext_model = 0;
    int ext_family = 0;
  } processor;

  struct Miscellaneous {
    int brand_id = 0;
    int clflush_size = 0;
    int logical_cpus = 0;
    int apic_id = 0;
  } misc;

  uint64_t features = 0;
};

void run(Function function, Result &result);
void read(CPU &cpu);

} // namespace arch::x86_64::cpuid