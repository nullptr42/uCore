; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .data

global _wakeup_start
global _wakeup_end

; GDT access byte defines
GDT_PRESENT equ (0x80)
GDT_SEGMENT equ (0x10)
GDT_EXEC    equ (0x08)
GDT_RW      equ (0x02)
GDT_CODE    equ (GDT_RW|GDT_EXEC)
GDT_DATA    equ (GDT_RW)

; GDT flag byte defines
GDT_GRAN equ (0x80)
GDT_PM   equ (0x40)
GDT_LM   equ (0x20)

bits 16
align 4096
_wakeup_start:
;    mov ax, cs
;    cmp ax, 0
;    je .okay
;    jmp 0x0:0x8000
;.okay:
    ; Enable A20-gate (needs better method).
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Disable interrupts and NMIs.
    cli
    in  al, 0x70
    and al, 0x7F
    out 0x70, al

    ; Load GDT
    xor ax, ax
    mov ds, ax
    lgdt [0x8000 + .gdt_pointer - _wakeup_start]

    ; Set PE-bit in CR0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Jump into protected mode.
    jmp 0x8:(0x8000 + .pm - _wakeup_start)

bits 32
.pm:
    mov ax, 0x10
    mov ds, ax
    mov word [0xB8000], 0x0742
    jmp $

; Barebones GDT table required to enter long mode.
.gdt:
    ; Creates a standard GDT entry.
    ;   %1=base
    ;   %2=limit
    ;   %3=access byte
    ;   %4=flags
    %macro gdt_entry 4
        dw  %2 & 0xFFFF      ; limit (0-15)
        db (%1 >>  0) & 0xFF ; base (0-23)
        db (%1 >>  8) & 0xFF ; ...
        db (%1 >> 16) & 0xFF ; ...
        db GDT_PRESENT|GDT_SEGMENT | %3 ; access byte
        db ((%2 >> 16) & 0x0F) | %4     ; flags, limit (16-19)
        db ((%1 >> 24) & 0xFF)          ; base (24-31)
    %endmacro

    dq 0
    gdt_entry 0x0, 0xFFFFF, GDT_CODE, (GDT_GRAN|GDT_PM) ; 32-bit CS (selector=0x08)
    gdt_entry 0x0, 0xFFFFF, GDT_DATA, (GDT_GRAN|GDT_PM) ; 32-bit DS (selector=0x10)

.gdt_pointer:
    dw (.gdt_pointer - .gdt)
    dd (0x8000 + .gdt - _wakeup_start)

align 4096
_wakeup_end:
