/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <log.h>
#include <stddef.h>
#include <arch/x86_64/apic/mp.h>
#include <arch/x86_64/vm/vm.h>

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

int mp_core_count = -1;
struct mpc_cpu* mp_cores[MP_MAX_CORES];

void mp_init() {
    void* data = (void*)VM_BASE_PHYSICAL;
    struct mpc_pointer* ptr = NULL;
    struct mpc_table* config = NULL;

    /* TODO: we are scanning the entire first 1MiB.
     * However the specification allows us to narrow it down
     * to three specific memory areas.
     */
    for (int i = 0; i <= 0xFFFFC; i++) {
        if (*(uint32_t*)(data + i) == 0x5F504D5F) { /* _MP_ */
            ptr = data + i;
            klog(LL_DEBUG, "mp: MPC-pointer discovered (%p).", ptr);
            break;
        }
    }

    if (ptr == NULL) {
        klog(LL_WARN, "mp: Missing MPC-pointer. Not a multicore system?");
        return;
    }

    config = data + ptr->config_ptr;
    klog(LL_DEBUG, "mp: MPC-table found (%p).", config);
    if (config->magic != 0x504D4350) {
        klog(LL_WARN, "mp: MPC-table signature mismatch: %#08x", config->magic);
        return;
    }

    int cpu_id = 0;
    void* entry = (void*)config + sizeof(struct mpc_table) + 1;

    /* Traverse configuration table. */
    for (int i = 0; i < config->entry_cnt; i++) {
        uint8_t type = *(uint8_t*)entry;
        switch (type) {
            case 0: {
                struct mpc_cpu* cpu = entry;
                if (cpu_id < MP_MAX_CORES) {
                    mp_cores[cpu_id] = cpu;
                }
                klog(LL_INFO, "cpu[%d]: lapic_id=%#x enabled=%u bsp=%u signature=%#x",
                    cpu_id++,
                    cpu->lapic_id,
                    cpu->enabled,
                    cpu->bsp,
                    cpu->signature
                );
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
                klog(LL_WARN, "mp: Unknown MPC-table entry type %#x.", type);
                break;
        }
    }

    mp_core_count = cpu_id;

    if (cpu_id > MP_MAX_CORES) {
        klog(LL_WARN, "mp: Got %d cpus but can support at maximum %d.", cpu_id, MP_MAX_CORES);
    }
}
