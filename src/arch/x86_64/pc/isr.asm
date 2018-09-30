; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

SEG_KDATA equ 0x10

global isr_test

isr_test:
    ; Save general purpose registers.
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rbx
    push rdx
    push rcx
    push rax

    ; Save segment registers (ds, fs, gs)
    ; However it is unclear if we will ever need fs or gs.
    mov ax, ds
    push ax
    push fs
    push gs

    ; Permit data access with kernel permissions.
    mov ax, SEG_KDATA,
    mov ds, ax
    mov fs, ax
    mov gs, ax

    ; Restore segment registers
    pop gs
    pop fs
    pop ax
    mov ds, ax

    ; Restore general purpose registers.
    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rsi
    pop rdi 
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    iretq
