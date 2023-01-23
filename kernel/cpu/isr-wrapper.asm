extern isr_page_fault
extern mem_handle_page_fault

[BITS 64]
section .isr

; The function is called with these values on the stack:
;
;  rsp + 40: Return SS (2/8)
;  rsp + 32: Return RSP
;  rsp + 24: Return RFLAGS
;  rsp + 16: Return CS (2/8)
;  rsp +  8: Return RIP
;  rsp +  0: Error Code (4/8) - only for exceptions with error code
;
; If there is no error we push a 0 error code
; Then we save all registers:
;
;  rsp +  0: rax
;  rsp +  0: rbx
;  rsp +  0: rcx
;  rsp +  0: rdx
;  rsp +  0: rsi
;  rsp +  0: rdi
;  rsp +  0: rbp
;  rsp +  0: rsp
;  rsp +  0: r8
;  rsp +  0: r9
;  rsp +  0: r10
;  rsp +  0: r11
;  rsp +  0: r12
;  rsp +  0: r13
;  rsp +  0: r14
;  rsp +  0: r15

isr_page_fault:

;    push 0             ; push a bogus error code if the exception has none

    push rax            ; save all registers
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push rsp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15            ; stack is 16-aligned now

    push 0              ; The System V ABI requires rsp be 8-aligned
                        ; push any 8 more bytes to 8-align the stack

    mov  rdi, cr2
    cld
    call mem_handle_page_fault

    pop r15             ; pop the alignment value

    pop r15             ; restore all registers
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsp
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 8          ; pop the error code off the stack

    iretq
