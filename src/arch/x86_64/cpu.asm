; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global gdt_reload
global idt_reload
global _cpuid
global rdmsr
global fpu_init

CR0_MONITOR_COPROC equ (1<< 1)
CR4_OSFXSR         equ (1<< 9)
CR4_OSXMMEXCPT     equ (1<<10)

; Reload GDT register
;   rdi(arg0) = GDT pointer
;   rsi(arg1) = code segment
;   rdx(arg2) = data segment
gdt_reload:
    lgdt [rdi]

    ; Load segments
    mov rax, rdx
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far return
    mov rax, .gdt_done
    push rsi
    push rax
    db 0x48 ; REX-prefix
    retf
.gdt_done:
    ret

; Reload IDT table
;   rdi(arg0) = IDT pointer
idt_reload:
    lidt [rdi]
    ret

; Get processor information
;   rdi(arg0): cpuid function
;   rsi(arg1): output structure
_cpuid:
    push rbx
    
    mov eax, edi
    cpuid
    mov [rsi + 0x0], eax
    mov [rsi + 0x4], ebx
    mov [rsi + 0x8], ecx
    mov [rsi + 0xC], edx

    pop rbx
    ret

; Read contents of a Model Specific Register (MSR).
;   rdi(arg0): register id
;   rsi(arg1): eax output address
;   rdx(arg2): edx output address 
rdmsr:
    mov r8, rdx
    
    ; Read register
    mov ecx, edi
    rdmsr

    ; Store result and return.
    mov [rsi], eax
    mov [r8], edx
    ret

fpu_init:
    ; Enable coprocessor monitoring.
    ; "Controls interaction of WAIT/FWAIT instructions with TS flag in CR0"
    mov rax, cr0
    ; and ax, 0xFFFB
    or ax, CR0_MONITOR_COPROC
    mov cr0, rax

    ; Enable FXSAVE and FXRSTOR instructions.
    mov rax, cr4
    or ax, (CR4_OSFXSR|CR4_OSXMMEXCPT)
    mov cr4, rax
    
    ret
