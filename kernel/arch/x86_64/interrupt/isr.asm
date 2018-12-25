; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

SEG_KDATA equ 0x10

extern handle_interrupt

; Stub for ISR without error code
%macro isr_stub 1
    global isr_routine_%1
    isr_routine_%1:
        push 0
        push %1
        jmp handle_irq_common
%endmacro

; Stub for ISR where an error code is on the stack
%macro isr_stub_err 1
    global isr_routine_%1
    isr_routine_%1:
        push %1
        jmp handle_irq_common
%endmacro

handle_irq_common:
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
    push gs
    push fs
    push ax

    ; Permit data access with kernel permissions.
    mov ax, SEG_KDATA,
    mov ds, ax
    mov fs, ax
    mov gs, ax

    mov rdi, rsp
    call handle_interrupt
    mov rsp, rax

    ; Restore segment registers
    pop ax
    mov ds, ax
    pop fs
    pop gs

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

    ; Remove error code & interrupt number from stack.
    add rsp, 16

    iretq

; Stubs for exceptions
isr_stub_err 0x00
isr_stub_err 0x01
isr_stub_err 0x02
isr_stub_err 0x03
isr_stub_err 0x04
isr_stub_err 0x05
isr_stub_err 0x06
isr_stub_err 0x07
isr_stub_err 0x08
isr_stub_err 0x09
isr_stub_err 0x0A
isr_stub_err 0x0B
isr_stub_err 0x0C
isr_stub_err 0x0D
isr_stub_err 0x0E
isr_stub_err 0x0F
isr_stub_err 0x10
isr_stub_err 0x11
isr_stub_err 0x12
isr_stub_err 0x13
isr_stub_err 0x14
isr_stub_err 0x15
isr_stub_err 0x16
isr_stub_err 0x17
isr_stub_err 0x18
isr_stub_err 0x19
isr_stub_err 0x1A
isr_stub_err 0x1B
isr_stub_err 0x1C
isr_stub_err 0x1D
isr_stub_err 0x1E
isr_stub_err 0x1F

; Stubs for Hardware IRQs
isr_stub 0x20
isr_stub 0x21
isr_stub 0x22
isr_stub 0x23
isr_stub 0x24
isr_stub 0x25
isr_stub 0x26
isr_stub 0x27
isr_stub 0x28
isr_stub 0x29
isr_stub 0x2A
isr_stub 0x2B
isr_stub 0x2C
isr_stub 0x2D
isr_stub 0x2E
isr_stub 0x2F
