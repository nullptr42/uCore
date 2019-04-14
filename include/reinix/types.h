/*
 * Copyright (C) 2019 Frederic Meyer (fleroviux). All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _REINIX_TYPES_H_
#define _REINIX_TYPES_H_

#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __x86_64__
typedef uint64_t addr_t;
#else
#error "Unknown CPU architecture, either not detected or unsupported by reinix."
#endif

typedef addr_t paddr_t;
typedef addr_t vaddr_t;
typedef unsigned int uint;

#endif // _REINIX_TYPES_H_