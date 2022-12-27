MAGIC       equ 0xe85250d6
ARCH        equ 0
HEADER_SIZE equ header_end - header_start
CHECKSUM    equ -(MAGIC + ARCH + HEADER_SIZE)
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

section .text
global _start:function (_end - _start)
_start:
    mov esp, stack_top

    push 0    ; Reset EFLAGS.
    popf

    push ebx  ; Push the pointer to the Multiboot information structure.
    push eax  ; Push the magic value.

    extern kernel_main  ; Now enter the C main function...
    call kernel_main

loop:
    hlt
    jmp loop

_end:
