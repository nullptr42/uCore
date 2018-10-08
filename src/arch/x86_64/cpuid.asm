; # --------------------------------------------------------------------- #
; Copyright (C) 2018-present Frederic Meyer. All rights reserved.         |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global _cpuid

; CPUID function
; Arguments:
;   rdi(arg0): cpuid function
;   rsi(arg1): output structure
_cpuid:
    mov eax, edi
    
    cpuid
    mov [rsi + 0x0], eax
    mov [rsi + 0x4], ebx
    mov [rsi + 0x8], ecx
    mov [rsi + 0xC], edx
    
    ret
