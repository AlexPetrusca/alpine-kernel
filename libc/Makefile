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

ARCH_CFLAGS=-m64 -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ARCH_CPPFLAGS=
ARCH_ASMFLAGS=-felf64
KERNEL_ARCH_CFLAGS=
KERNEL_ARCH_CPPFLAGS=
KERNEL_ARCH_ASMFLAGS=

ARCH_FREEOBJS=\

ARCH_HOSTEDOBJS=\

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
ASMFLAGS:=$(ASMFLAGS) $(ARCH_ASMFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_CFLAGS)
LIBK_CPPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_CPPFLAGS)
LIBK_ASMFLAGS:=$(ASMFLAGS) $(KERNEL_ASMFLAGS)

FREEOBJS=\
$(ARCH_FREEOBJS) \
stdio/printf.o \
stdio/printf_test.o \
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
sys/circ_buf.o \
sys/circ_buf_ptr.o \
sys/dll.o \
sys/vector.o

HOSTEDOBJS=\
$(ARCH_HOSTEDOBJS) \

OBJS=\
$(FREEOBJS) \
$(HOSTEDOBJS) \

LIBK_OBJS=$(FREEOBJS:.o=.libk.o)

#BINARIES=libc.a libk.a # Not ready for libc yet.
BINARIES=libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S .asm

all: $(BINARIES)

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS)

.c.S:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.asm:
	$(ASM) $< -o $@ $(ASMFLAGS)

.c.libk.o:
	$(CC) -MD -c $< -o $@ -std=gnu17 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.S.libk.o:
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.asm.libk.o:
	$(ASM) $< -o $@ $(ASMFLAGS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R -p include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
