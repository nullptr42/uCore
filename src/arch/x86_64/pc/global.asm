; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

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
