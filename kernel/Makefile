.ONESHELL: # Applies to every targets in the file!

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS) -z noexecstack
LIBS:=$(LIBS) -nostdlib -lk -lgcc

KERNEL_CFLAGS=-m64 -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -mno-sse2
KERNEL_CPPFLAGS=
KERNEL_ASMFLAGS=-felf64
KERNEL_LDFLAGS=
KERNEL_LIBS=

CFLAGS:=$(CFLAGS) $(KERNEL_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_CPPFLAGS)
ASMFLAGS:=$(ASMFLAGS) $(KERNEL_ASMFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_LIBS)

KERNEL_OBJS=\
boot/kernel.o \
boot/boot.o \
boot/mb2_info.o \
tty/tty.o \
tty/vga_ttyd.o \
tty/vbe_ttyd.o \
font/psf_font.o \
font/Default.o \
font/Fixed13.o \
font/Fixed14.o \
font/Fixed15.o \
font/Fixed16.o \
font/Fixed18.o \
font/Terminus12x6.o \
font/Terminus14.o \
font/Terminus16.o \
font/Terminus18x10.o \
font/Terminus20x10.o \
font/Terminus22x11.o \
font/Terminus24x12.o \
font/Terminus28x14.o \
font/Terminus32x16.o \
font/TerminusBold14.o \
font/TerminusBold16.o \
font/TerminusBold18x10.o \
font/TerminusBold20x10.o \
font/TerminusBold22x11.o \
font/TerminusBold24x12.o \
font/TerminusBold28x14.o \
font/TerminusBold32x16.o \
font/TerminusBoldVGA14.o \
font/TerminusBoldVGA16.o \
shell/sh.o \
cpu/msr.o \
cpu/cpu.o \
cpu/smp.o \
cpu/trampoline.o \
cpu/process.o \
cpu/gdt.o \
cpu/idt.o \
cpu/pic.o \
cpu/isr.o \
cpu/isr-wrapper.o \
device/acpi.o \
device/apic.o \
device/pci.o \
device/usb.o \
mem/mem.o \
mem/heap.o \
mem/heap_test.o \
test/tests.o \
test/test.o \
mem/pgm.o \
mem/pgm_test.o \
boot/elf.o \

OBJS=\
boot/crti.o \
crtbegin.o \
$(KERNEL_OBJS) \
crtend.o \
boot/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS) \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .asm .psf

all: alpine.kernel

alpine.kernel: $(OBJS) linker.ld
	@ cd ../build
	$(CC) -T ../kernel/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot2 alpine.kernel

crtbegin.o crtend.o:
	@ OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" ../build/$@

cpu/isr.o:
	@ cd ../build
	@ mkdir -p $(@D);
	$(CC) -MD -c ../kernel/cpu/isr.c -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS) -mgeneral-regs-only

.c.o:
	@ cd ../build
	@ mkdir -p $(@D);
	$(CC) -MD -c ../kernel/$< -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS)

.S.o:
	@ cd ../build
	@ mkdir -p $(@D);
	$(CC) -MD -c ../kernel/$< -o $@ $(CFLAGS) $(CPPFLAGS)

.psf.o:
	@ cd ../build
	@ mkdir -p $(@D);
	cp ../kernel/$< $(@D);
	x86_64-elf-objcopy -O elf64-x86-64 -B i386 -I binary $< $@

.asm.o:
	@ cd ../build
	@ mkdir -p $(@D);
	$(ASM) ../kernel/$< -o $@ $(ASMFLAGS)

clean:
	@ rm -f build
	@ rm -f alpine.kernel
	@ rm -f $(OBJS) *.o */*.o */*/*.o
	@ rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	@ mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@ cp -R -p include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: alpine.kernel
	@ mkdir -p $(DESTDIR)$(BOOTDIR)
	@ cd ../build;
	@ cp alpine.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
