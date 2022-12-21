cmake_minimum_required(VERSION 3.21)
project(alpine_kernel C)

set(CMAKE_C_STANDARD 11)

include_directories(kernel/include)
include_directories(kernel/include/kernel)
include_directories(libc/include)
include_directories(libc/include/sys)
include_directories(/usr/include/efi)
include_directories(/usr/include/efi/x86_64)
include_directories(/usr/include/efi/protocol)

add_executable(alpine_kernel
        kernel/arch/i386/sh.c
        kernel/arch/i386/tty.c
        kernel/arch/i386/msr.c
        kernel/arch/i386/cpu.c
        kernel/include/kernel/kb.h
        kernel/include/kernel/sh.h
        kernel/include/kernel/tty.h
        kernel/include/kernel/vga.h
        kernel/include/kernel/msr.h
        kernel/include/kernel/cpu.h
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
        libc/stdio/sprintf.c
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
        libc/sys/circ_buf.c)
