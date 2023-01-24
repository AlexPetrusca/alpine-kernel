global idt
global idt_size
extern main_isr

; TODO There is a strange bug where if this number is large the kernel fails...
ISR_COUNT equ 128

%macro PUSH 1
    mov rax, %1
    push rax
%endmacro

%macro POP 1
    pop rax
    mov %1, rax
%endmacro

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
; If there is no error we push a 0 error code.
; Then we save all registers.

    %assign i 0
    %rep ISR_COUNT

isr_no_err_%+i:       ; entry point for an ISR without error code
    push 0            ; push a bogus error code if the exception has none

isr_err_%+i:          ; entry point for an ISR with error code
    push i            ; push the vector number
    jmp  start_isr

    %assign i i + 1
    %endrep

  start_isr:
    push rax            ; save all registers
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    push fs
    push gs
    PUSH ds
    PUSH es

    PUSH cr4
    PUSH cr2
    PUSH cr0
    PUSH cr3

    mov  rdi, rsp       ; pass the frame pointer as the first arg
;    push 0              ; The SystemV ABI requires rsp be 8-aligned, push 8 bytes
    call main_isr
;    pop rax

    ; restore all registers
    POP cr3
    POP cr0
    POP cr2
    POP cr4

    POP es
    POP ds
    pop gs
    pop fs

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 8          ; pop the vector number off the stack
    add rsp, 8          ; pop the error code off the stack

    iretq


section .idt

%macro idt_no_err_entry 1
    dq isr_no_err_%+%1  ; the address of the ISR function
    dq 0
%endmacro

%macro idt_err_entry 1
    dq isr_err_%+%1  ; the address of the ISR function
    dq 0
%endmacro

idt_size:
    dw ISR_COUNT

idt:
    idt_no_err_entry 0     ; List the first 32 vectors
    idt_no_err_entry 1
    idt_no_err_entry 2
    idt_no_err_entry 3
    idt_no_err_entry 4
    idt_no_err_entry 5
    idt_no_err_entry 6
    idt_no_err_entry 7
    idt_err_entry    8
    idt_no_err_entry 9
    idt_err_entry    10
    idt_err_entry    11
    idt_err_entry    12
    idt_err_entry    13
    idt_err_entry    14
    idt_no_err_entry 15
    idt_no_err_entry 16
    idt_err_entry    17
    idt_no_err_entry 18
    idt_no_err_entry 19
    idt_no_err_entry 20
    idt_no_err_entry 21
    idt_no_err_entry 22
    idt_no_err_entry 23
    idt_no_err_entry 24
    idt_no_err_entry 25
    idt_no_err_entry 26
    idt_no_err_entry 27
    idt_no_err_entry 28
    idt_no_err_entry 29
    idt_err_entry    30
    idt_no_err_entry 31

    %assign i 32        ; List the remaining vectors, 32 - ISR_COUNT
    %rep ISR_COUNT - 32
    idt_no_err_entry i  ; the address of the ISR function
    %assign i i + 1
    %endrep
