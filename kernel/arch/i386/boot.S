#define ASM_FILE    1
#include <kernel/multiboot2.h>

#define STACK_SIZE  0x4000

.section .multiboot

header_start:
    .align 8
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT_ARCHITECTURE_I386
    .long header_end - header_start
    .long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (header_end - header_start))

//framebuffer_tag:
//    .align  8
//    .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
//    .short MULTIBOOT_HEADER_TAG_OPTIONAL
//    .long framebuffer_tag_end - framebuffer_tag_start
//    .long 1024
//    .long 768
//    .long 32

end_tag:
    .align 8
    .short MULTIBOOT_HEADER_TAG_END
    .short 0
    .long 8

header_end:

.section .text
.global _start
.type _start, @function

_start:
    movl $(stack + STACK_SIZE), %esp    /* Initialize the stack pointer. */

    pushl $0    /* Reset EFLAGS. */
    popf

    pushl %ebx  /* Push the pointer to the Multiboot information structure. */
    pushl %eax  /* Push the magic value. */
    call kernel_main  /* Now enter the C main function... */

loop:
    hlt
    jmp loop

.comm stack, STACK_SIZE /* Stack area, 16KB. */