; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

global _start
extern kernel_main

section .text

    _start:
        mov rsp, stack_top
        call kernel_main
        cli
    .loop:
        hlt
        jmp .loop

section .bss

    align 16
    stack_bottom:
        resb 32768 ; 32 KiB
    stack_top: