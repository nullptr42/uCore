/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_PM_H_
#define _X86_64_PC_PM_H_

#include "stack.h"
#include <bootinfo.h>

void pm_init(struct bootinfo* binf);

#endif //_X86_64_PC_PM_H_
