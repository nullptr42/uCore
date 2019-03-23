#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MB2_BOOTLOADER_MAGIC (0x36D76289)

namespace multiboot {

struct Header {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed));

enum class TagType : uint32_t {
    CommandLine = 1,
    Module = 3,
    Memory = 4,
    MemoryMap = 6,
    Framebuffer = 8
};

struct Tag {
    TagType type;
    uint32_t size;
} __attribute__((packed));

struct ModuleTag : Tag {
    uint32_t mod_start;
    uint32_t mod_end;
    char string; /* array */
} __attribute__((packed));

struct MemoryTag : Tag {
    /* in kilobytes */
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed));

struct MemoryMapTag : Tag {
    uint32_t entry_size;
    uint32_t entry_version;
} __attribute__((packed));

/*enum mb2_mmap_type {
    MB_MMAP_RESERVED  = 0,
    MB_MMAP_AVAILABLE = 1,
    MB_MMAP_ACPI_INFO = 3,
    MB_MMAP_HIBERNATE = 4,
    MB_MMAP_DEFECTIVE = 5
};*/

enum class MemoryMapType : uint32_t {
    Reserved = 0,
    Available = 1,
    AcpiInfo = 3,
    Hibernate = 4,
    Defective = 5
};

struct MemoryMapEntry {
    uint64_t base;
    uint64_t length;
    MemoryMapType type;
    uint32_t reserved;
} __attribute__((packed));

struct FramebufferTag : Tag {
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  type;
    uint16_t reserved; /* according to documentation this should
                        * be uint8_t but it only works like this ¯\_(ツ)_/¯ */

    /* TFW assuming we are only using non-indexed mode. ;) */
    uint8_t r_shift;
    uint8_t r_mask_len;
    uint8_t g_shift;
    uint8_t g_mask_len;
    uint8_t b_shift;
    uint8_t b_mask_len;
} __attribute__((packed));

bool multiboot2_verify(uint32_t magic);

typedef bool (*Callback)(Tag* tag, void* user_argument);

void multiboot2_find_tags(Header* info,
                          TagType type,
                          Callback handler,
                          void* user_argument
                         );

}

