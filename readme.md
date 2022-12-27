# Alpine Kernel

A minimal x86 kernel

## Install

1. Install cross compiler:`brew install i686-elf-gcc x86_64-elf-gcc objconv`
2. Clone GRUB: `git clone git://git.savannah.gnu.org/grub.git`
3. Run GRUB's autogen: `cd grub && ./autogen.sh`
4. Configure GRUB for i686-elf: `../grub/configure --disable-werror TARGET_CC=i686-elf-gcc TARGET_OBJCOPY=i686-elf-objcopy
   TARGET_STRIP=i686-elf-strip TARGET_NM=i686-elf-nm TARGET_RANLIB=i686-elf-ranlib --target=i686-elf --program-prefix=i686-elf-`
5. Build and install i686-elf-grub tools: `make && make install`
6. Configure GRUB for x86_64-elf: `../grub/configure --disable-werror TARGET_CC=x86_64-elf-gcc TARGET_OBJCOPY=x86_64-elf-objcopy
   TARGET_STRIP=x86_64-elf-strip TARGET_NM=x86_64-elf-nm TARGET_RANLIB=x86_64-elf-ranlib --target=x86_64-elf --program-prefix=x86_64-elf-`
7. Build and install x86_64-elf-grub tools: `make && make install`