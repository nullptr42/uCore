; # --------------------------------------------------------------------- #
; Copyright (C) 2019 Frederic Meyer. All rights reserved.                 |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global gdt_reload
global idt_reload

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
