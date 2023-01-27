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
KERNEL_DEBUG_CFLAGS=-s -fno-asynchronous-unwind-tables
KERNEL_CPPFLAGS=
KERNEL_ASMFLAGS=-felf64
KERNEL_LDFLAGS=
KERNEL_LIBS=

CFLAGS:=$(CFLAGS) $(KERNEL_CFLAGS) $(KERNEL_DEBUG_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_CPPFLAGS)
ASMFLAGS:=$(ASMFLAGS) $(KERNEL_ASMFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_LIBS)

OBJS=\
build/crtbegin.o \
build/crtend.o \
build/boot/crti.o \
build/boot/crtn.o \
build/boot/elf.o \
build/boot/kernel.o \
build/boot/boot.o \
build/boot/mb2_info.o \
build/tty/tty.o \
build/tty/vga_ttyd.o \
build/tty/vbe_ttyd.o \
build/font/psf_font.o \
build/font/Default.o \
build/font/Fixed13.o \
build/font/Fixed14.o \
build/font/Fixed15.o \
build/font/Fixed16.o \
build/font/Fixed18.o \
build/font/Terminus12x6.o \
build/font/Terminus14.o \
build/font/Terminus16.o \
build/font/Terminus18x10.o \
build/font/Terminus20x10.o \
build/font/Terminus22x11.o \
build/font/Terminus24x12.o \
build/font/Terminus28x14.o \
build/font/Terminus32x16.o \
build/font/TerminusBold14.o \
build/font/TerminusBold16.o \
build/font/TerminusBold18x10.o \
build/font/TerminusBold20x10.o \
build/font/TerminusBold22x11.o \
build/font/TerminusBold24x12.o \
build/font/TerminusBold28x14.o \
build/font/TerminusBold32x16.o \
build/font/TerminusBoldVGA14.o \
build/font/TerminusBoldVGA16.o \
build/shell/sh.o \
build/cpu/msr.o \
build/cpu/cpu.o \
build/cpu/smp.o \
build/cpu/trampoline.o \
build/cpu/process.o \
build/cpu/gdt.o \
build/cpu/idt.o \
build/cpu/pic.o \
build/cpu/isr.o \
build/cpu/isr-wrapper.o \
build/device/acpi.o \
build/device/apic.o \
build/device/pci.o \
build/device/usb.o \
build/device/kb.o \
build/mem/mem.o \
build/mem/heap.o \
build/mem/heap_test.o \
build/mem/pgm.o \
build/mem/pgm_test.o \
build/test/tests.o \
build/test/test.o \

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS) \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .asm .psf

all: build/alpine.kernel

build/alpine.kernel: $(OBJS) linker.ld
	@ $(CC) -T linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	@ grub-file --is-x86-multiboot2 build/alpine.kernel

build/crtbegin.o build/crtend.o:
	@ mkdir -p $(@D);
	@ OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

build/%.o : %.c
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c $< -o $@ -std=gnu17 $(CFLAGS) $(CPPFLAGS)

build/%.o : %.S
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

build/%.o : %.psf
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	cp ../kernel/$< $(@D);
	x86_64-elf-objcopy -O elf64-x86-64 -B i386 -I binary $< $@

build/%.o : %.asm
	@ echo "Compiling "$<"..."
	@ mkdir -p $(@D);
	$(ASM) $< -o $@ $(ASMFLAGS)

clean:
	@ rm -f -r build
	@ rm -f alpine.kernel
	@ rm -f $(OBJS) *.o */*.o */*/*.o
	@ rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	@ mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@ cp -R -p include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: build/alpine.kernel
	@ mkdir -p $(DESTDIR)$(BOOTDIR)
	@ cp build/alpine.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
