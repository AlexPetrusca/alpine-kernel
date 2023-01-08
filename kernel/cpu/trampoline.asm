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

; Paging constants
PML4T_START   equ 0xA000

extern trampoline_start
extern trampoline_end
extern smp_idt_pointer_size
extern smp_idt_pointer_base
extern smp_gdt_pointer_size
extern smp_gdt_pointer_base
extern smp_process_table

section .trampolinedata
align 4
idt:
    .Length:
        dw 0
    .Base:
        dd 0

gdt:
    .Null:
        dq 0
    .Code:
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19) (64-bit long mode)
        db 0                                        ; Base (high, bits 24-31)
    .Data:
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Pointer:
        dw $ - gdt - 1
        dq gdt

misc_data:
    .SmpIdtPointer:
        smp_idt_pointer_size    dw 0            ; size of the IDT
        smp_idt_pointer_base    dd 0            ; pointer to the IDT

    .SmpGdtPointer:
        smp_gdt_pointer_size    dw 23           ; size of the GDT
        smp_gdt_pointer_base    dq 0x10F00E     ; pointer to the GDT

    .SmpPml4Pointer:
        smp_pml4                dq 0xA000       ; pointer to the PML4

    counter                 dw 0
    smp_process_table       dq 0
    offset                  dw 40

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

    lidt [idt]                        ; Load a zero length IDT so that any NMI causes a triple fault.

    ; Enter long mode.
    mov ecx, IA32_EFER_MSR            ; Set the LME bit in the EFER MSR.
    rdmsr
    or eax, IA32_EFER_MSR_LME
    wrmsr

    mov eax, CR4_PAE | CR4_PGE | CR4_MCE | CR4_OSFXSR | CR4_OSXMMEXCPT
    mov cr4, eax

    mov edi, PML4T_START              ; Point CR3 at the PML4.
    mov cr3, edi

    mov ebx, cr0                      ; Activate long mode -
    or ebx, CR0_PE | CR0_PG           ; - by enabling paging and protection simultaneously.
    mov cr0, ebx

    lgdt [gdt.Pointer]                ; Load GDT pointer

    jmp CODE_SEG:LongMode

[BITS 64]
LongMode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Blank out the screen to a blue color.
    mov edi, 0xB8000
    mov rcx, 500                      ; Since we are clearing uint64_t over here, we put the count as Count/4.
    mov rax, 0x1F201F201F201F20       ; Set the value to set the screen to: Blue background, white foreground, blank spaces.
    rep stosq                         ; Clear the entire screen.

    ; Display "Hello World!"
    mov edi, 0x00b8000
    mov rax, 0x1F6C1F6C1F651F48
    mov [edi],rax
    mov rax, 0x1F6F1F571F201F6F
    mov [edi + 8], rax
    mov rax, 0x1F211F641F6C1F72
    mov [edi + 16], rax

    pause: jmp pause

;    extern kernel_main
;    call kernel_main   ; Now enter the C main function...

.loop:
    hlt
    jmp .loop

trampoline_end:
