.ONESHELL: # Applies to every targets in the file!

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS)
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_libk

CFLAGS=-m64 -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CPPFLAGS=
ASMFLAGS=-felf64
KERNEL_CFLAGS=
KERNEL_CPPFLAGS=
KERNEL_ASMFLAGS=

CFLAGS:=$(CFLAGS) $(CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(CPPFLAGS)
ASMFLAGS:=$(ASMFLAGS) $(ASMFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_CFLAGS)
LIBK_CPPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_CPPFLAGS)
LIBK_ASMFLAGS:=$(ASMFLAGS) $(KERNEL_ASMFLAGS)

OBJS=\
build/sys/dll_test.o \
build/sys/strsplit_test.o \
build/stdio/printf_test.o \
build/string/memchr_test.o \
build/string/strchr_test.o \
build/string/strstr_test.o \
build/string/strtok_test.o \
build/string/strcat_test.o \
build/string/strdup_test.o \
build/string/strtrim_test.o \
build/stdio/printf.o \
build/stdio/getchar.o \
build/stdio/putchar.o \
build/stdio/puts.o \
build/stdlib/malloc.o \
build/stdlib/abort.o \
build/stdlib/itoa.o \
build/stdlib/atoi.o \
build/stdlib/strtoimax.o \
build/stdlib/strtol.o \
build/stdlib/strtoul.o \
build/string/memchr.o \
build/string/memcmp.o \
build/string/memcpy.o \
build/string/memmove.o \
build/string/memset.o \
build/string/strcpy.o \
build/string/strrev.o \
build/string/strlen.o \
build/string/strcmp.o \
build/string/strequ.o \
build/string/strchr.o \
build/string/strtok.o \
build/string/strspn.o \
build/string/strstr.o \
build/string/strcat.o \
build/string/strdup.o \
build/string/strtrim.o \
build/sys/circ_buf.o \
build/sys/circ_buf_ptr.o \
build/sys/dll.o \
build/sys/vector.o \
build/sys/vector_test.o \
build/sys/strsplit.o \
build/errno/errno.o

LIBK_OBJS=$(OBJS:.o=.libk.o)

#BINARIES=libc.a libk.a # Not ready for libc yet.
BINARIES=build/libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S .asm

all: $(BINARIES)

libc.a: $(OBJS)
	@ $(AR) rcs $@ $(OBJS)

build/libk.a: $(LIBK_OBJS)
	@ $(AR) rcs $@ $(LIBK_OBJS)

build/%.o : %.c
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c $< -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS)

build/%.o : %.S
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

build/%.o : %.asm
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(ASM) $< -o $@ $(ASMFLAGS)

build/%.libk.o : %.c
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ -std=gnu17 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

build/%.libk.o : %.S
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

build/%.libk.o : %.asm
	@ mkdir -p $(@D);
	$(ASM) ../libc/$< -o $@ $(ASMFLAGS)

clean:
	@ rm -f -r build
	@ rm -f $(BINARIES) *.a
	@ rm -f $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	@ rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	@ mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@ cp -R -p include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	@ mkdir -p $(DESTDIR)$(LIBDIR)
	@ cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
