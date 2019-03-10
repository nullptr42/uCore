
#pragma once

#define SEG_KCODE_LM (0x08)
#define SEG_UCODE_LM (0x1B)
#define SEG_KDATA    (0x10)
#define SEG_UDATA    (0x23)

void gdt_init();
