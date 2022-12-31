; multiboot2 constants
MAGIC       equ 0xe85250d6
ARCH        equ 0   ; x86 arch
HEADER_SIZE equ multiboot_header_end - multiboot_header
CHECKSUM    equ -(MAGIC + ARCH + HEADER_SIZE)

; multiboot2 tag constants
FRAMEBUFFER_TAG         equ 5
FRAMEBUFFER_TAG_SIZE    equ 20

; CR0 bits
CR0_PG   equ 1 << 31
CR0_PE   equ 1 << 0

; CR4 bits
CR4_PAE         equ 1 << 5
CR4_MCE         equ 1 << 6
CR4_PGE         equ 1 << 7
CR4_OSFXSR      equ 1 << 9
CR4_OSXMMEXCPT  equ 1 << 10

; MSR bits
IA32_EFER_MSR      equ  0xc0000080
IA32_EFER_MSR_LME  equ  1 << 8

; GDT segment selectors
CODE_SEG    equ 0x0008
DATA_SEG    equ 0x0010

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
PAGE_SIZE     equ 4096   ; 4 KB
PML4T_START   equ 0x1000
PDPT_START    equ 0x2000
PDT_START     equ 0x3000
PT_START      equ 0x4000

; Page access bits
PAGE_PRESENT    equ (1 << 0)
PAGE_WRITE      equ (1 << 1)

; Stack constants
STACK_SIZE      equ 16384   ; 16 KB

; Screen constants
SCREEN_WIDTH    equ 1024
SCREEN_HEIGHT   equ 768
SCREEN_DEPTH    equ 32

section .multiboot
multiboot_header:
    align 8
    dd MAGIC
    dd ARCH
    dd HEADER_SIZE
    dd CHECKSUM

framebuffer_tag:
    align 8
    dw FRAMEBUFFER_TAG
    dw 0
    dd FRAMEBUFFER_TAG_SIZE
    dd SCREEN_WIDTH
    dd SCREEN_HEIGHT
    dd SCREEN_DEPTH

end_tag:
    align 8
    dw 0
    dw 0
    dd 8
multiboot_header_end:

section .bss
align 16
stack_bottom:
resb STACK_SIZE ; 16 KiB stack
stack_top:

section .data
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

[BITS 32]
section .text
global _start:function (_end - _start)
_start:
    mov esp, stack_top

    push 0  ; Reset EFLAGS.
    popf

    push 0  ; Push the 64-bit pointer to the Multiboot information structure.
    push ebx
    push 0  ; Push the 64-bit magic value.
    push eax

    ; Switch to long mode

    ; Zero out the 16KiB buffer.
    mov edi, PML4T_START
    xor eax, eax
    mov ecx, PAGE_SIZE      ; 4096 * 4 = 16k
    cld
    rep stosd               ; Clear the memory.

    ; Build the PML4T, PDPT and PDT
    mov edi, PML4T_START
    mov DWORD [edi], PDPT_START | PAGE_PRESENT | PAGE_WRITE   ; PML4T points to PDPT
    mov edi, PDPT_START
    mov DWORD [edi], PDT_START | PAGE_PRESENT | PAGE_WRITE    ; PDPT points to PDT
    mov edi, PDT_START
    mov DWORD [edi], PT_START | PAGE_PRESENT | PAGE_WRITE     ; PDT points to PT

    ; Build the PT table identify mapping the first 2Mb
    mov edi, PT_START
    mov ebx, PAGE_PRESENT | PAGE_WRITE
    mov ecx, 512                 ; map 512 4k pages = 2Mb
.SetEntry:
    mov DWORD [edi], ebx
    add ebx, PAGE_SIZE           ; move to the next 4k page
    add edi, 8
    loop .SetEntry

    ; Disable IRQs
    mov al, 0xFF                 ; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out 0xA1, al
    out 0x21, al

    nop
    nop

    lidt [IDT]                   ; Load a zero length IDT so that any NMI causes a triple fault.

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

    lgdt [GDT.Pointer]                ; Load GDT.Pointer defined below.

    jmp CODE_SEG:LongMode             ; Load CS with 64 bit segment (CS selects the submode of long mode)

[BITS 64]
LongMode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi  ; pop the magic value
    pop rsi  ; pop the pointer to the Multiboot information structure

    extern kernel_main
    call kernel_main   ; Now enter the C main function...

loop:
    hlt
    jmp loop

_end:
