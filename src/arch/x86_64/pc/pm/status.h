/*
 * Copyright (C) 2018-present Frederic Meyer. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _X86_64_PC_PM_STATUS_H_
#define _X86_64_PC_PM_STATUS_H_

enum pmm_status {
    PMM_OUT_OF_MEMORY = -2,
    PMM_BAD_REQUEST = -1,
    PMM_OK = 0
};

#endif //_X86_64_PC_PM_STATUS_H_
