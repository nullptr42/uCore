; # --------------------------------------------------------------------- #
; Copyright (C) 2019 Frederic Meyer. All rights reserved.                 |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global fpu_init

CR0_MONITOR_COPROC equ (1<< 1)
CR4_OSFXSR         equ (1<< 9)
CR4_OSXMMEXCPT     equ (1<<10)

fpu_init:
    finit

    ; Enable coprocessor monitoring.
    ; "Controls interaction of WAIT/FWAIT instructions with TS flag in CR0"
    mov rax, cr0
    and ax, 0xFFFB
    or ax, CR0_MONITOR_COPROC
    mov cr0, rax

    ; Enable FXSAVE and FXRSTOR instructions.
    mov rax, cr4
    or ax, (CR4_OSFXSR|CR4_OSXMMEXCPT)
    mov cr4, rax

    ret
