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

extern trampoline_start
extern trampoline_end
extern smp_idt_pointer_size
extern smp_idt_pointer_base
extern smp_gdt_pointer_size
extern smp_gdt_pointer_base
extern smp_process_table

;section .bss2
;align 16
;stack_bottom:
;resb 1024
;stack_top:

[BITS 32]
align 32
section .trampoline

trampoline_start:
  cli
  cld
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

;  mov esp, stack_top
;  mov ebp, esp
;  push 0  ; Reset EFLAGS.
;  popf

  ; Enable A20 Gate
;  in al, 0x92
;  or al, 0x02
;  out 0x92, al

  ; Get an unique id
;  mov eax, dword [counter]
;  try:
;    mov ebx, eax
;    inc ebx
;    ; Compares ax with [counter], if equal: [counter] = bx & z=1, if not ax = [counter] & z=0
;    lock cmpxchg dword [counter], ebx
;    jnz try
;  mov edi, eax          ; Save the unique id

  ; Disable all IRQs by writing 0xFF to 0xA1 and 0x21.
  mov al, 0xFF
  out 0xA1, al
  out 0x21, al
  nop
  nop

  lidt [IDT]             ; Load a zero length IDT so that any NMI causes a triple fault.

  ; Enter long mode.
  mov ecx, IA32_EFER_MSR            ; Set the LME bit in the EFER MSR.
  rdmsr
  or eax, IA32_EFER_MSR_LME
  wrmsr

  mov eax, CR4_PAE | CR4_PGE | CR4_MCE | CR4_OSFXSR | CR4_OSXMMEXCPT
  mov eax, 0x6E0
  mov cr4, eax

  mov edi, smp_pml4                    ; Point CR3 at the PML4.
  mov cr3, edi

  ; Write Hello World! to the text buffer
  [BITS 16]
  mov eax, 0b800h
  mov es, eax
  mov edi, [offset]
  mov dword [es:edi + 0], 0x1F651F48
  mov dword [es:edi + 4], 0x1F6C1F6C
  mov dword [es:edi + 8], 0x1F201F6F
  mov dword [es:edi + 12], 0x1F6F1F57
  mov dword [es:edi + 16], 0x1F6C1F72
  mov dword [es:edi + 20], 0x1F211F64
  [BITS 32]

  mov ebx, cr0                      ; Activate long mode -
  or ebx, CR0_PE | CR0_PG | CR0_CD          ; - by enabling paging and protection simultaneously.
;    or ebx, CR0_PE | CR0_PG           ; - by enabling paging and protection simultaneously.
  mov ebx, 0x080000011
    mov cr0, ebx

  lgdt [GDT.Pointer]                 ; Load GDT pointer

;[BITS 64]
;  lea rax, [rel LongMode2]
;  push CODE_SEG
;  push rax
;  retf

 jmp CODE_SEG:LongMode2             ; Load CS with 64 bit segment (CS selects the submode of long mode)

[BITS 64]
align 64
LongMode2:

    mov eax, DATA_SEG
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    ; Display "Hello World!"
    mov edi, 0x00b8000
    mov rax, 0x1F6C1F6C1F651F48
    mov [edi],rax
    mov rax, 0x1F6F1F571F201F6F
    mov [edi + 8], rax
    mov rax, 0x1F211F641F6C1F72
    mov [edi + 16], rax

    jmp HALT

;    extern kernel_main
;    call kernel_main   ; Now enter the C main function...

HALT:
    cli
loop:
    hlt
    jmp loop

section .trampolinedata
align 4

IDT:
    .Length:
        dw 0
    .Base:
        dd 0

GDT:
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
        dw $ - GDT - 1
        dq GDT

smp_idt_pointer:
smp_idt_pointer_size    dw 0 ; size of the IDT
smp_idt_pointer_base    dd 0 ; pointer to the IDT

smp_gdt_pointer:
smp_gdt_pointer_size    dw 23 ; size of the GDT
smp_gdt_pointer_base    dq 0x10F00E ; pointer to the GDT

smp_pml4                dq 0xA000 ; pointer to the PML4

counter                 dw 0
smp_process_table       dq 0
offset                  dw 40

trampoline_end:
