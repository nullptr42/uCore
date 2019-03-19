#pragma once

#include <stdint.h>
#include <kernel/bootinfo.hpp>

kernel::BootInfo* get_bootinfo(uint32_t magic, void* multiboot);
