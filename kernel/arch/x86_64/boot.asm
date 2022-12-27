MAGIC       equ 0xe85250d6
ARCH        equ 0
HEADER_SIZE equ header_end - header_start
CHECKSUM    equ -(MAGIC + ARCH + HEADER_SIZE)

PAGE_PRESENT    equ (1 << 0)
PAGE_WRITE      equ (1 << 1)

CODE_SEG    equ 0x0008
DATA_SEG    equ 0x0010

; GDT Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; GDT Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

STACK_SIZE  equ 0x4000

section .multiboot

header_start:
    align 8
    dd MAGIC
    dd ARCH
    dd HEADER_SIZE
    dd CHECKSUM

; framebuffer_tag:
;     align  8
;     dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
;     dw MULTIBOOT_HEADER_TAG_OPTIONAL
;     dd framebuffer_tag_end - framebuffer_tag_start
;     dd 1024
;     dd 768
;     dd 32

end_tag:
    align 8
    dw 0
    dw 0
    dd 8

header_end:

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
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
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

section .text
global _start:function (_end - _start)
_start:
    mov esp, stack_top

    push 0    ; Reset EFLAGS.
    popf

    push ebx  ; Push the pointer to the Multiboot information structure.
    push eax  ; Push the magic value.

    jmp SwitchToLongMode

    extern kernel_main  ; Now enter the C main function...
    call kernel_main
;
;loop:
;    hlt
;    jmp loop

; Function to switch directly to long mode from real mode.
; Identity maps the first 2MiB.
; Uses Intel syntax.
SwitchToLongMode:
    ; Zero out the 16KiB buffer.
    ; Since we are doing a rep stosd, count should be bytes/4.
    push di                           ; REP STOSD alters DI.
    mov ecx, 0x1000
    xor eax, eax
    cld
    rep stosd
    pop di                            ; Get DI back.

    ; Build the Page Map Level 4.
    ; es:di points to the Page Map Level 4 table.
    lea eax, [es:di + 0x1000]         ; Put the address of the Page Directory Pointer Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di], eax                  ; Store the value of EAX as the first PML4E.

    ; Build the Page Directory Pointer Table.
    lea eax, [es:di + 0x2000]         ; Put the address of the Page Directory in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di + 0x1000], eax         ; Store the value of EAX as the first PDPTE.

    ; Build the Page Directory.
    lea eax, [es:di + 0x3000]         ; Put the address of the Page Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writeable flag.
    mov [es:di + 0x2000], eax         ; Store to value of EAX as the first PDE.

    push di                           ; Save DI for the time being.
    lea di, [di + 0x3000]             ; Point DI to the page table.
    mov eax, PAGE_PRESENT | PAGE_WRITE    ; Move the flags into EAX - and point it to 0x0000.

; Build the Page Table.
.LoopPageTable:
    mov [es:di], eax
    add eax, 0x1000
    add di, 8
    cmp eax, 0x200000                 ; If we did all 2MiB, end.
    jb .LoopPageTable

    pop di                            ; Restore DI.

    ; Disable IRQs
    mov al, 0xFF                      ; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out 0xA1, al
    out 0x21, al

    nop
    nop

    lidt [IDT]                        ; Load a zero length IDT so that any NMI causes a triple fault.

    ; Enter long mode.
    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax

    mov edx, edi                      ; Point CR3 at the PML4.
    mov cr3, edx

    mov ecx, 0xC0000080               ; Read from the EFER MSR.
    rdmsr

    or eax, 0x00000100                ; Set the LME bit.
    wrmsr

    mov ebx, cr0                      ; Activate long mode -
    or ebx, 0x80000001                 ; - by enabling paging and protection simultaneously.
    mov cr0, ebx

    lgdt [GDT.Pointer]                ; Load GDT.Pointer defined below.

    jmp CODE_SEG:LongMode             ; Load CS with 64 bit segment and flush the instruction cache

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
    mov [edi], rax

    mov rax, 0x1F6F1F571F201F6F
    mov [edi + 8], rax

    mov rax, 0x1F211F641F6C1F72
    mov [edi + 16], rax

    spin: jmp spin

    push rbx  ; Push the pointer to the Multiboot information structure.
    push rax  ; Push the magic value.
    extern kernel_main  ; Now enter the C main function...
    call kernel_main

loop:
    hlt
    jmp loop

_end:
