; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .data

global _wakeup_start
global _wakeup_end
global _wakeup_tab

extern vm_level1
extern ap_main

; TODO: this is redundant to boot.asm
VM_BASE_KERNEL_ELF equ 0xFFFFFFFF80000000

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
    jmp start
_wakeup_tab:
    .stack: dq 0
    .pml4: dq 0
start:
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
    ; Disable paging initially
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax

    ; Enable Physical Address Extension (PAE)
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    
    ; It cannot be guaranteed that the kernel PML4
    ; lies within the first 4 GiB of memory.
    ; Thus we load the bootstrap PML4 first and later
    ; we switch to the kernel PML4.
    mov eax, vm_level1 - VM_BASE_KERNEL_ELF
    mov cr3, eax

    ; Enable long mode (LM)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    jmp 0x18:(0x8000 + .lm - _wakeup_start)
bits 64
.lm:
    mov rax, .fixed_rip
    jmp rax
.fixed_rip:
    mov rsp, [0x8000 + (_wakeup_tab.stack - _wakeup_start)]
    mov rdx, [0x8000 + (_wakeup_tab.pml4 - _wakeup_start)]
    mov cr3, rdx
    call ap_main
    cli
.stop:
    hlt
    jmp .stop

align 4096
.stack:
    times 4096 db 0
.stack_top:

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
    gdt_entry 0x0, 0xFFFFF, GDT_CODE, (GDT_GRAN|GDT_LM) ; 64-bit CS (selector=0x18)

.gdt_pointer:
    dw (.gdt_pointer - .gdt)
    dd (0x8000 + .gdt - _wakeup_start)

align 4096
_wakeup_end:
