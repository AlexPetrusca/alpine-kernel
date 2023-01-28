global idt
global idt_size
extern main_isr

ISR_COUNT equ 256

%macro PUSH 1
    mov rax, %1
    push rax
%endmacro

%macro POP 1
    pop rax
    mov %1, rax
%endmacro

%macro ALLOC 1
  sub rsp, %1
%endmacro

%macro FREE 1
  add rsp, %1
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
; If there is no error we push a 0 error code. Then we save all registers.
;
; TODO Better FPU/MMX/SSE save strategy:
; The processor does not automatically save the context of the
; FPU, XMM, and MXCSR registers on a task switch. Instead, it sets the
; TS flag, which causes the processor to raise an #NM exception whenever
; it encounters an FPU/MMX/SSE instruction in the instruction stream for
; the new task. The fault handler for the #NM exception can then be used
; to clear the TS flag (with the CLTS instruction) and save the context
; of the FPU, XMM, and MXCSR registers. If the task never encounters an
; FPU/MMX/SSE instruction the context is never saved.

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
    ALLOC 8             ; fxsave requires 16-alignment of the stack
    ALLOC 512           ; allocate 512 bytes on stack for FPU/MMX/SSE registers
    fxsave [rsp]        ; save FPU/MMX/SSE registers

    push rax            ; save general registers
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

    push fs             ; save segment registers
    push gs
    PUSH ds
    PUSH es

    PUSH cr4            ; save control registers
    PUSH cr2
    PUSH cr0
    PUSH cr3

    mov  rdi, rsp       ; pass the frame pointer as the first arg
    ALLOC 8             ; The SystemV ABI requires rsp be 8-aligned, push 8 bytes
    call main_isr
    FREE  8             ; Pop the SystemV ABI 8-aligned value

    POP cr3             ; restore control registers
    POP cr0
    POP cr2
    POP cr4

    POP es               ; restore segment registers
    POP ds
    pop gs
    pop fs

    pop r15              ; restore general registers
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

    fxrstor [rsp]   ; restore FPU/MMX/SSE registers
    FREE 512        ; free the FPU/MMX/SSE space from the stack
    FREE 8          ; pop fxsave 16-alignment value

    FREE 8          ; pop vector number off the stack (added by CPU or by us)
    FREE 8          ; pop error code off the stack (added by the CPU)

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
