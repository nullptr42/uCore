
/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "apic.h"
#include <stdint.h>
#include <stddef.h>
#include <arch/print.h>
#include <arch/x86_64/pc/vm/vm.h>

#define MSR_APIC_BASE  (0x1B)
#define MSR_APIC_BASE_ENABLE (0x800)
#define MSR_APIC_BASE_BSP (0x100)

void _rdmsr(uint32_t reg, uint32_t* eax, uint32_t* edx);
void _wrmsr(uint32_t reg, uint32_t  eax, uint32_t  edx);

static void* lapic_mmio = NULL;

bool lapic_is_present(struct amd64_cpu* cpu) {
    return cpu->features & CPUID_FEAT_APIC;
}

static uint64_t lapic_get_base() {
    uint32_t lapic_base_edx;
    uint32_t lapic_base_eax;

    /* get current LAPIC base from MSR */
    _rdmsr(MSR_APIC_BASE, &lapic_base_eax, &lapic_base_edx);

    return ((uint64_t)lapic_base_edx<<32)|(lapic_base_eax&0xFFFFF000);
}

static void lapic_set_base(uint64_t base) {
    base = (base&0xFFFFF000)|MSR_APIC_BASE_ENABLE|MSR_APIC_BASE_BSP;
    _wrmsr(MSR_APIC_BASE, (base&0xFFFFFFFF), (base>>32));
}

/* TODO: place MPC code somewhere else? */

/* Multiprocessor Configuration Table */
struct mpc_table {
    uint32_t magic;
    uint16_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[8];
    char     product_id[12];
    uint32_t oem_tab;
    uint16_t oem_tab_sz;
    uint16_t entry_cnt;
    uint32_t lapic_base;
    uint16_t ext_tab_sz;
    uint8_t  ext_tab_chk;
} __attribute__((packed));

/* Multiprocessor Floating Pointer Structure */
struct mpc_pointer {
    uint32_t magic;
    uint32_t config_ptr;
    uint8_t  length;
    uint8_t  version;
    uint8_t  checksum;
    uint8_t  features[5];
} __attribute__((packed));

/* Multiprocessor Processor Entry */
struct mpc_cpu {
    uint8_t type;
    uint8_t lapic_id;
    uint8_t lapic_ver;
    unsigned int enabled : 1;
    unsigned int bsp : 1;
    unsigned int reserved : 6;
    uint32_t signature;
    uint32_t features;
} __attribute__((packed));

void delay(int ms);

static void wakeup(int apic_id) {
    volatile uint32_t* icr_hi = (void*)lapic_mmio + 0x310;
    volatile uint32_t* icr_lo = (void*)lapic_mmio + 0x300;
    volatile uint32_t* id = (void*)lapic_mmio + 0x20;
    
    *icr_hi = apid_id << 24;
    *icr_lo = 0x11 | (5 << 8) | (1 << 14) | (0 << 18);
    *id;
    delay(10);
    while (icr_lo[0] & (1<<12)) kprintf("doing it");
    *icr_lo = 0x11 | (6 << 8) | (0 << 14) | (0 << 18);
    *id;
    delay(10);
    while (icr_lo[0] & (1<<12)) kprintf("doing it");
}

static void find_mpc_table() {
    void* data = (void*)0xFFFF808000000000;
    struct mpc_pointer* ptr = NULL;
    struct mpc_table* config = NULL;

    /* TODO: atm we are scanning the entire first 1MiB.
     * However the specification allows us to narrow it down
     * to three specific memory areas.
     */
    for (int i = 0; i <= 0xFFFFC; i++) {
        if (*(uint32_t*)(data + i) == 0x5F504D5F) { /* _MP_ */
            ptr = data + i;
            info("lapic: Floating Pointer Structure @ %p", ptr);
            break;
        }
    }

    if (ptr == NULL) {
        warn("lapic: Unable to locate Floating Pointer Structure.");
        return;
    }

    config = data + ptr->config_ptr;
    info("lapic: MP Configuration Table @ %p", config);

    if (config->magic != 0x504D4350) {
        warn("lapic: Config table signature mismatch: %#08x", config->magic);
        return;
    }

    trace("lapic: Entry Count = %d", config->entry_cnt);

    int cpu_id = 0;
    /* Why the fuck do we have to + 1?
     * The entries are said to _follow_ after the config table.
     * Our structure has exactly 43 bytes which matches the specified size.
     */
    void* entry = (void*)config + sizeof(struct mpc_table) + 1;

    /* Traverse configuration table. */
    for (int i = 0; i < config->entry_cnt; i++) {
        uint8_t type = *(uint8_t*)entry;
        switch (type) {
            case 0: {
                struct mpc_cpu* cpu = entry;
                kprintf("cpu[%d]: lapic_id=%#x enabled=%u bsp=%u signature=%#x\n",
                    cpu_id++,
                    cpu->lapic_id,
                    cpu->enabled,
                    cpu->bsp,
                    cpu->signature
                );
                wakeup(cpu->lapic_id);
                entry += 20;
                break;
            }
            case 1:
            case 2:
            case 3:
            case 4:
                entry += 8;
                break;
            default:
                entry += 8;
                warn("lapic: Unknown config entry type %#x.", type);
                break;
        }
    }
}

void lapic_init() {
    uint64_t base = lapic_get_base();

    /* Enable the Local-APIC */
    info("lapic: Base address is 0x%016llX", base);
    lapic_set_base(base);
    info("lapic: Enabled Local-APIC.");

    /* Map its MMIO registers into memory */
    lapic_mmio = vm_alloc(1);
    vm_map_page(lapic_mmio, base / 4096);
    info("lapic: Mapped Local-APIC MMIO @ %p", lapic_mmio);

    volatile uint32_t* spivr = (void*)lapic_mmio + 0xF0;

    /* Actually enabling the Local-APIC by setting
     * Enable-bit in the SPIVR register.
     */
    *spivr |= 0x80;
    find_mpc_table();

    uint8_t payload[] = {
        0xB8, 0x00, 0xB8, /* mov ax, 0xB800 */
        0x8E, 0xD8,       /* mov ds, ax */
        0xC7, 0x06, 0x00, 0x00, 0x41, 0x07, /* mov word [0], 0x0741 */
        0xEB, 0xFE        /* jmp $ */
    };
    uint8_t* dst = (void*)0xFFFF808000000000 + 0x11000;

    info("lapic: copying payload of size %zd to 0x1000.", sizeof(payload));
    for (int i = 0; i < sizeof(payload); i++) {
        dst[i] = payload[i];
    }

    kprint("halting now");
    while (1) {}

    kprintf("completed jizz\n");
}
