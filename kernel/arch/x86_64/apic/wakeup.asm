; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global _wakeup_start
global _wakeup_end
global _wakeup_tab

extern vm_kctx
extern vm_level1
extern ap_main

%include "arch/x86_64/gdt.inc"
%include "arch/x86_64/global.inc"

bits 16
align 4096
_wakeup_start:
    jmp start
_wakeup_tab:
    .stack: dq 0
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
    mov rdx, vm_kctx - VM_BASE_KERNEL_ELF
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
    dq 0
    gdt_entry 0x0, 0xFFFFF, GDT_CODE, (GDT_GRAN|GDT_PM) ; 32-bit CS (selector=0x08)
    gdt_entry 0x0, 0xFFFFF, GDT_DATA, (GDT_GRAN|GDT_PM) ; 32-bit DS (selector=0x10)
    gdt_entry 0x0, 0xFFFFF, GDT_CODE, (GDT_GRAN|GDT_LM) ; 64-bit CS (selector=0x18)

.gdt_pointer:
    dw (.gdt_pointer - .gdt)
    dd (0x8000 + .gdt - _wakeup_start)

align 4096
_wakeup_end:
