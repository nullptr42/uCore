; # --------------------------------------------------------------------- #
; Copyright (C) 2019 Frederic Meyer. All rights reserved.                 |
;                                                                         |
; Use of this source code is governed by a BSD-style license that can be  |
; found in the LICENSE file.                                              |
; # --------------------------------------------------------------------- #

section .text

global cpu_rdmsr
global cpu_wrmsr

; Read contents of a Model Specific Register (MSR).
;   rdi(arg0): register id
;   rsi(arg1): eax output address
;   rdx(arg2): edx output address
cpu_rdmsr:
    mov r8, rdx

    ; Read register
    mov ecx, edi
    rdmsr

    ; Store result and return.
    mov [rsi], eax
    mov [r8], edx
    ret

; Write contents of a Model Specific Register (MSR).
;   rdi(arg0): register id
;   rsi(arg1): eax value
;   rdx(arg2): edx value
cpu_wrmsr:
    mov ecx, edi
    mov eax, esi
    wrmsr
    ret
