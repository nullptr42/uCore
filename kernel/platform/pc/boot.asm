; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

global _start

; Multiboot 2 constants.
MAGIC equ 0xE85250D6
ARCH  equ 0

; Paging constants
PAGE_IN_MEMORY equ (1<<0)
PAGE_WRITEABLE equ (1<<1)
PAGE_SIZE_2MB  equ (1<<7)

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

global vm_level2_b ; TODO: do we need this?
global vm_level1

section .multiboot

    ; Multiboot 2 Header, must be aligned to 64-bit boundary.
    ; DOCS:
    ;    https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
    ;    https://wiki.osdev.org/Multiboot#Multiboot_2
    align 8
    mb_start:
        dd MAGIC               ; Multiboot 2 magic number
        dd ARCH                ; architecture (i386/x86)
        dd (mb_end - mb_start) ; header length
        dd 0x100000000 - (MAGIC + ARCH + (mb_end - mb_start)) ; checksum, prove we are multiboot 2

        ; Request Module Information
        dw 1  ; type
        dw 0  ; flags
        dd 12 ; size (bytes)
        dd 3  ; request module information
        dd 0  ; padding

%ifdef BOOT_GX
        ; Request Framebuffer
        dw 5  ; type
        dw 0  ; flags
        dd 20 ; size (bytes)
        dd 0  ; width
        dd 0  ; height
        dd 32 ; bit depth
        dd 0  ; padding
%endif

        ; End of Tags
        dw 0 ; type
        dw 0 ; flags
        dd 8 ; size
    mb_end:

section .text

    ; Load preliminary GDT table, containing a long mode code segment.
    [BITS 32]
    gdt_init:
        lgdt [gdt_pointer - VM_BASE_KERNEL_ELF]

        ; Load segments
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp long 0x08:(.gdt_done - VM_BASE_KERNEL_ELF)
    .gdt_done:
        ret

    [BITS 32]
    compat_barrier:
        mov eax, 0x80000001
        cpuid
        test edx, (1<<29)
        jnz .return

        ; Print "AMD64 cpu required. Cannot boot."
        mov dword [0xB8000], 'AxMx'
        mov dword [0xB8004], 'Dx6x'
        mov dword [0xB8008], '4x x'
        mov dword [0xB800C], 'cxpx'
        mov dword [0xB8010], 'ux x'
        mov dword [0xB8014], 'rxex'
        mov dword [0xB8018], 'qxux'
        mov dword [0xB801C], 'ixrx'
        mov dword [0xB8020], 'exdx'
        mov dword [0xB8024], '.x x'
        mov dword [0xB8028], 'Cxax'
        mov dword [0xB802C], 'nxnx'
        mov dword [0xB8030], 'oxtx'
        mov dword [0xB8034], ' xbx'
        mov dword [0xB8038], 'oxox'
        mov dword [0xB803C], 'tx.x'
        cli
    .loop:
        hlt
        jmp .loop
    .return:
        ret

    extern kernel_main

    VM_BASE_KERNEL_ELF equ 0xFFFFFFFF80000000

    ; ENTRYPOINT
    [BITS 32]
    _start:
        mov esp, stack_top - VM_BASE_KERNEL_ELF

        ; Setup arguments for kernel_main.
        ; This must be the first step because we are destroying eax and ebx afterwards.
        ;   edi=magic number
        ;   esi=multiboot header
        mov edi, eax
        mov esi, ebx

        call compat_barrier

        ; Disable paging (in case it already was enabled)
        mov eax, cr0        ; read CR0
        and eax, 0x7FFFFFFF ; clear paging bit
        mov cr0, eax        ; write back to CR0

        call gdt_init

        ; Enable Physical Address Extension (PAE)
        mov eax, cr4  ; read CR4
        or eax, 0x20  ; set PAE-bit
        mov cr4, eax  ; write back to CR4

        ; Setup level 3 which will map to the first 1GiB of physical memory.
        mov eax, vm_level3 - VM_BASE_KERNEL_ELF
        mov ecx, 512
        xor edx, edx
        .map_1gb:
            or dword [eax], edx
            add eax, 8
            add edx, 0x200000 ; 2 MiB
            dec ecx
            jnz .map_1gb

        or dword [vm_level2_a.entry - VM_BASE_KERNEL_ELF], vm_level3 - VM_BASE_KERNEL_ELF
        or dword [vm_level2_b.entry - VM_BASE_KERNEL_ELF], vm_level3 - VM_BASE_KERNEL_ELF

        or dword [vm_level1.lo - VM_BASE_KERNEL_ELF], vm_level2_a - VM_BASE_KERNEL_ELF ; 0x0000000000000000
        or dword [vm_level1.hi - VM_BASE_KERNEL_ELF], vm_level2_b - VM_BASE_KERNEL_ELF ; 0xFFFFFFFF80000000

        mov eax, vm_level1 - VM_BASE_KERNEL_ELF
        mov cr3, eax

        ; Enable long mode (LM)
        mov ecx, 0xC0000080  ; MSR register to access
        rdmsr                ; read MSR value
        or eax, 0x000000100  ; set LM-bit
        wrmsr                ; write back to MSR

        ; Enable paging
        mov eax, cr0        ; read CR0
        or eax, 0x80000001  ; set paging and protected mode bits
        mov cr0, eax        ; write back to CR0

        jmp long 0x18:do_call - VM_BASE_KERNEL_ELF
    [BITS 64]
    do_call:
        mov rsp, stack_top
        mov rax, kernel_main
        call rax
    stop:
        cli
    .loop:
        hlt
        jmp .loop

section .rodata

    ; GDT pointer struct as required by the lgdt [...] instruction...
    gdt_pointer:
        dw (8*4)-1   ; 4 entries á 8 bytes
        dd gdt_table - VM_BASE_KERNEL_ELF ; address of GDT table

    ; Barebones GDT table required to enter long mode.
    gdt_table:
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

section .data

    align 4096
    vm_level3:
        times 512 dq PAGE_IN_MEMORY | PAGE_WRITEABLE | PAGE_SIZE_2MB

    vm_level2_a:
        .entry: dq PAGE_IN_MEMORY | PAGE_WRITEABLE
        times 511 dq 0

    vm_level2_b:
        times 510 dq 0
        .entry: dq PAGE_IN_MEMORY | PAGE_WRITEABLE
        dq 0

    vm_level1:
        ; 0x0000000000000000
        .lo:
            dq PAGE_IN_MEMORY | PAGE_WRITEABLE
            times 510 dq 0
        ; 0xFFFFFFFF80000000
        .hi:
            dq PAGE_IN_MEMORY | PAGE_WRITEABLE

section .bss

    align 16
    stack_bottom:
        resb 32768 ; 32 KiB
    stack_top:
