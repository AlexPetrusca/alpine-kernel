# Alpine Kernel

A minimal x86 kernel

## Install

1. Install cross compiler:`brew install x86_64-elf-gcc objconv`
2. Clone GRUB: `git clone git://git.savannah.gnu.org/grub.git`
3. Run GRUB's autogen: `cd grub && ./autogen.sh`
4. Configure GRUB build: `../grub/configure --disable-werror TARGET_CC=x86_64-elf-gcc TARGET_OBJCOPY=x86_64-elf-objcopy
      TARGET_STRIP=x86_64-elf-strip TARGET_NM=x86_64-elf-nm TARGET_RANLIB=x86_64-elf-ranlib --target=x86_64-elf`
5. Build and install GRUB: `make && make install`

## Live USB

1. Download and install [Etcher](https://etcher.io/)
2. Run `./iso.sh` to create an iso image under `build`
3. Run Etcher and flash `alpine.iso` to usb device

## Debug

* Get a disassembly of the kernel 
  * `x86_64-elf-objdump -D kernel/alpine.kernel > kernel.dump`
* Get debug logs and cpu traces from Qemu
  * Enable logging: `./qemu.sh -D qemu.log`
  * Log interrupts/exceptions: add `-d int`
  * Log cpu state before reset: add `-d cpu_reset`
  * Get list of supported logs: `qemu-system-x86_64 -d --help`