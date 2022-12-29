cmake_minimum_required(VERSION 3.21)
project(alpine_kernel C)

set(CMAKE_C_STANDARD 17)

include_directories(kernel/include)
include_directories(kernel/include/kernel)
include_directories(libc/include)
include_directories(libc/include/sys)
include_directories(/usr/include/efi)
include_directories(/usr/include/efi/x86_64)
include_directories(/usr/include/efi/protocol)

add_executable(alpine_kernel
        kernel/arch/x86_64/boot.asm
        kernel/arch/x86_64/sh.c
        kernel/arch/x86_64/tty.c
        kernel/arch/x86_64/msr.c
        kernel/arch/x86_64/cpu.c
        kernel/arch/x86_64/acpi.c
        kernel/arch/x86_64/apic.c
        kernel/arch/x86_64/pci.c
        kernel/include/kernel/kb.h
        kernel/include/kernel/sh.h
        kernel/include/kernel/tty.h
        kernel/include/kernel/vga.h
        kernel/include/kernel/msr.h
        kernel/include/kernel/cpu.h
        kernel/include/kernel/acpi.h
        kernel/include/kernel/Acpi10.h
        kernel/include/kernel/Acpi20.h
        kernel/include/kernel/apic.h
        kernel/include/kernel/pci.h
        kernel/kernel/kernel.c
        libc/include/sys/cdefs.h
        libc/include/sys/circ_buf.h
        libc/include/sys/io.h
        libc/include/ctype.h
        libc/include/errno.h
        libc/include/stdio.h
        libc/include/stdlib.h
        libc/include/string.h
        libc/stdio/printf.c
        libc/stdio/putchar.c
        libc/stdio/puts.c
        libc/stdlib/abort.c
        libc/stdlib/itoa.c
        libc/string/memchr.c
        libc/string/memcmp.c
        libc/string/memcpy.c
        libc/string/memmove.c
        libc/string/memset.c
        libc/string/strcat.c
        libc/string/strchr.c
        libc/string/strcmp.c
        libc/string/strcpy.c
        libc/string/strequ.c
        libc/string/strlen.c
        libc/string/strrev.c
        libc/string/strspn.c
        libc/string/strstr.c
        libc/string/strtok.c
        libc/sys/circ_buf.c
        kernel/kernel/test.c
        kernel/include/test.h
        libc/stdio/printf_test.c
        libc/include/stdio_tests.h
        kernel/include/kernel/multiboot2.h kernel/arch/x86_64/mem.c kernel/include/kernel/mem.h)
