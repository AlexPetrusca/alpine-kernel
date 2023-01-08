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
sys/dll_test.o \
sys/strsplit_test.o \
stdio/printf_test.o \
string/memchr_test.o \
string/strchr_test.o \
string/strstr_test.o \
string/strtok_test.o \
string/strcat_test.o \
string/strdup_test.o \
stdio/printf.o \
stdio/getchar.o \
stdio/putchar.o \
stdio/puts.o \
stdlib/malloc.o \
stdlib/abort.o \
stdlib/itoa.o \
string/memchr.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strcpy.o \
string/strrev.o \
string/strlen.o \
string/strcmp.o \
string/strequ.o \
string/strchr.o \
string/strtok.o \
string/strspn.o \
string/strstr.o \
string/strcat.o \
string/strdup.o \
sys/circ_buf.o \
sys/circ_buf_ptr.o \
sys/dll.o \
sys/vector.o \
sys/strsplit.o

LIBK_OBJS=$(OBJS:.o=.libk.o)

#BINARIES=libc.a libk.a # Not ready for libc yet.
BINARIES=libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S .asm

all: $(BINARIES)

libc.a: $(OBJS)
	@ cd ../build
	$(AR) rcs $@ $(OBJS)

libk.a: $(LIBK_OBJS)
	@  cd ../build
	$(AR) rcs $@ $(LIBK_OBJS)

.c.o:
	@ cd ../build mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS)

.c.S:
	@ cd ../build; mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.asm:
	@ cd ../build; mkdir -p $(@D);
	$(ASM) ../libc/$< -o $@ $(ASMFLAGS)

.c.libk.o:
	@ cd ../build; mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ -std=gnu17 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.S.libk.o:
	@ cd ../build; mkdir -p $(@D);
	$(CC) -MD -c ../libc/$< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.asm.libk.o:
	@ cd ../build; mkdir -p $(@D);
	$(ASM) ../libc/$< -o $@ $(ASMFLAGS)

clean:
	@ rm -f build
	@ rm -f $(BINARIES) *.a
	@ rm -f $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	@ rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	@ mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@ cp -R -p include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	@ mkdir -p $(DESTDIR)$(LIBDIR)
	@ cd ../build; cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
