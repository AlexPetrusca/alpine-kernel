; GDT segment selectors
CODE_SEG    equ 0x0008
DATA_SEG    equ 0x0010

; MSR bits
IA32_EFER_MSR      equ  0xc0000080
IA32_EFER_MSR_LME  equ  1 << 8

; CR0 bits
CR0_PE   equ 1 << 0
CR0_ET   equ 1 << 4
CR0_CD   equ 1 << 30
CR0_PG   equ 1 << 31

; CR4 bits
CR4_PAE         equ 1 << 5
CR4_MCE         equ 1 << 6
CR4_PGE         equ 1 << 7
CR4_OSFXSR      equ 1 << 9
CR4_OSXMMEXCPT  equ 1 << 10

; GDT access bits
ACCESSED      equ 1 << 0
RW            equ 1 << 1
DC            equ 1 << 2
EXEC          equ 1 << 3
NOT_SYS       equ 1 << 4
RING1         equ 1 << 5
RING2         equ 1 << 6
RING3         equ (1 << 5) | (1 << 6)
PRESENT       equ 1 << 7

; GDT flag bits
LONG_MODE     equ 1 << 5
SZ_32         equ 1 << 6
GRAN_4K       equ 1 << 7

extern smp_idt_pointer_size
extern smp_idt_pointer_base
extern smp_gdt_pointer_size
extern smp_gdt_pointer_base
extern smp_pml4_base
extern _processes;
extern enter_smp;

[BITS 16]
section .trampoline
trampoline_start:
    cli

    ; Disable IRQs
    mov al, 0xFF                      ; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out 0xA1, al
    out 0x21, al

    nop
    nop

    lidt [SmpIdtPointer]                        ; Load a zero length IDT so that any NMI causes a triple fault.

    ; Enter long mode.
    mov ecx, IA32_EFER_MSR            ; Set the LME bit in the EFER MSR.
    rdmsr
    or eax, IA32_EFER_MSR_LME
    wrmsr

    mov eax, CR4_PAE | CR4_PGE | CR4_MCE | CR4_OSFXSR | CR4_OSXMMEXCPT
    mov cr4, eax

    mov edi, [SmpPml4Pointer]              ; Point CR3 at the PML4.
    mov cr3, edi

    mov ebx, cr0                      ; Activate long mode -
    or ebx, CR0_PE | CR0_PG           ; - by enabling paging and protection simultaneously.
    mov cr0, ebx

    lgdt [SmpGdtPointer]                ; Load GDT pointer

    jmp CODE_SEG:enter_smp

[BITS 64]
enter_smp:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Get an unique id
    mov rax, [process_counter]
    try:
      mov rbx, rax
      inc rbx
      ; Compares ax with [counter], if equal: [counter] = bx & z=1, if not ax = [counter] & z=0
      lock cmpxchg [process_counter], rbx
      jnz try
    mov rdi, rax                           ; Save the unique id

    mov rax, rdi                           ; get the counter
    shl rax, 4                             ; multiply by 16 to index into the processes
    add rax, _processes                    ; add the base address of the process table

    mov rsp, [rax]                         ; read the stack pointer
    mov rbp, rsp

    call [rax + 8]                         ; call the process main function

.loop:
    hlt
    jmp .loop

align 4

SmpIdtPointer:
        smp_idt_pointer_size    dw 0            ; size of the IDT
        smp_idt_pointer_base    dd 0            ; pointer to the IDT

SmpGdtPointer:
        smp_gdt_pointer_size    dw 0           ; size of the GDT
        smp_gdt_pointer_base    dq 0           ; pointer to the GDT

SmpPml4Pointer:
        smp_pml4_base           dq 0           ; pointer to the PML4

        process_counter         dq 0

trampoline_end:
